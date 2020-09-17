# 前言

最近有个需求，所有本地海思的板子，都要能查询到他们的IP。如果是用串口调试，当然很容易知道IP，但是大部分情况下，我们还是用telnet远程连接，这时候，不知道IP就啥也干不了。

因此我想着在海思的板子上跑一个服务器程序，然后在本地运行客户端程序去查询，服务器接收到数据就将自己的IP返回给客户端，这样就知道所有海思芯片的IP了。

一开始是做了一个用TCP协议通信的，但是考虑到ABC三类IP加起来数量太多了，线程开太多，程序会把电脑卡死。然后又尝试了UDP广播，但是又不能做到跨网段，最终的解决方案是不使用C-S方式（客户端-服务器），而是采用对等连接方式P2P（peer to peer），在通信的时候，并不区分是服务器还是客户端，而是两台主机同时具有服务器和客户端的能力。并且发送信息的方式，都是往255.255.255.255广播，这样就能跨网段进行接收了。

本文档将所有码记录下来，并对关键点进行解释，方便以后查阅。

windows端所使用的线程API，是一个朋友封装的，文档中不提供代码，但是下载资源中可以找到

本文档资源地址：https://download.csdn.net/download/whitefish520/12831608

# 	TCP方式

## 服务器

服务器端监听所有IP，并在客户端接入后，创建线程，发送自己的IP

```cpp
//linux arm-hisiv500-linux-gcc server.c -o server -lpthread    

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<strings.h>
#include<ctype.h>
#include<stdlib.h>
#include<pthread.h>

void *run(void *arg);
void trim(char *strIn, char *strOut);

int main(){
		//lfd：socket返回的文件描述符，用lfd监听并接受客户端连接
   		//cfd：客户端链接上来返回的cfd，用cfd和客户端通信
        int lfd,cfd;
        int opt = 1;
		char client_ip[256];
        //serv_addr服务器地址
        //client_addr客户端地址
        struct sockaddr_in serv_addr,client_addr;
        socklen_t client_len;
        pthread_t tid;	
        
        //socket创建服务器返回lfd文件描述符
        lfd = socket(AF_INET,SOCK_STREAM,0);
        setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        bzero(&serv_addr,sizeof(serv_addr));
        //ipv4
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8080);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   
        //bind
        bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        //监听是否有客户连接
        listen(lfd,256);
       
        while(1){
           client_len = sizeof(client_addr);//客户端地址长度
           //连接成功返回cfd，用cfd和客户通信
           cfd = accept(lfd,(struct sockaddr*)&client_addr,&client_len);
		   printf("client:%s\t%d\n",
               inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_ip,sizeof(client_ip)),
		       ntohs(client_addr.sin_port));
		   //创建线程 回调 run方法  cfd做参数传入 
           pthread_create(&tid,NULL,run,(void *)cfd);  
           pthread_detach(tid);
		}
		return 0;
} 
void *run(void *arg){
	int cfd = (int ) arg;
	char buf[1024];//缓冲区
	char ipbuf[1024];//缓冲区
	int len,i;
	char *pinfo;
    FILE *fp;
    
	while(1){
		system("ifconfig -a | grep 'inet addr' > ipinfo");	//获取ip并写入文件
        fp = fopen("ipinfo", "r");
		if (fp == NULL)
		{
			printf("fopen error!\n");
			return;
		}
		while (!feof(fp)) 
      	{ 
			fgets(buf,1024,fp);  //从文件中一行一行读出数据
			if(!feof(fp)) {
				trim(buf, ipbuf);	//取出首尾的空格
				write(cfd,ipbuf,strlen(ipbuf));//发送给服务器
			}
      	} 
		fclose(fp);	
		system("rm ipinfo");	//发送完毕，删除ip信息文件
		break;
	}
	close(cfd);
}

//实现去除首尾空格的效果
void trim(char *strIn, char *strOut)
{
    int i, j ;
    i = 0;
    j = strlen(strIn) - 1;
    while(strIn[i] == ' ')
        ++i;
    while(strIn[j] == ' ')
        --j;
    strncpy(strOut, strIn + i , j - i + 1);
    strOut[j - i + 1] = '\0';
}
```



## 客户端

客户端程序做了对Windows的适配，需要Windows上安装MinGW gcc编译器，编译命令在注释中

客户端要求执行时输入一个参数，例如` ./client 192.168.10`，注意只要三段，客户端会对1-255尝试连接，成功则创建线程，打印信息

```cpp
//win32  gcc client.c pps_thread_win32.c -o client.exe  -lwsock32
//linux  gcc client.c -o client -lpthread   

#ifdef _WIN32
	#include <stdio.h>
	#include <winsock2.h>
	#include "pps_thread.h"
	#pragma comment(lib,"ws2_32.lib")
#else
	#include<stdio.h>
	#include<stdlib.h>
	#include<errno.h>
	#include<string.h>
	#include<sys/types.h>
	#include<netinet/in.h>
	#include<sys/socket.h>
	#include<sys/wait.h> 
#endif

#define DEST_PORT 8080//目标地址端口号 
#define MAX_DATA 100//接收到的数据最大程度 

char targetIP[32];
int stop = 0;
 
void *run(void *arg)
{
#ifdef _WIN32
	SOCKET sockfd;
	WSADATA wsaData;
	WORD wVersionRequired;
	wVersionRequired = MAKEWORD(2,2);
	if (WSAStartup(wVersionRequired, &wsaData)!= 0)
    {
        printf("init failed %ld\n",WSAGetLastError());
    }
#else
	int sockfd;
#endif
	int len,i;
	char serverIP[64];
	char endIP[256];
	int end = (int ) arg;
	sprintf(endIP, ".%d", end);
	strcpy(serverIP, targetIP);
	strcat(serverIP, endIP); 

    struct sockaddr_in dest_addr;

	char buf[MAX_DATA];
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	dest_addr.sin_family=AF_INET;
 	dest_addr.sin_port=htons(DEST_PORT);
	dest_addr.sin_addr.s_addr=inet_addr(serverIP);
	memset(dest_addr.sin_zero, 0, 8);
	connect(sockfd,(struct sockaddr*)&dest_addr,sizeof(struct sockaddr));
	while(1){
		char buf[512] = "";
#ifdef _WIN32
		len = recv(sockfd, buf, sizeof(buf), 0);
#else
		len = read(sockfd, buf, sizeof(buf));
#endif
		if(len<=0) 
			continue;
		else
    		printf("%s => %s",serverIP,buf);
   		if(strcmp(serverIP, "192.168.10.255"))
   			stop = 1;
	}
#ifdef _WIN32
	closesocket(sockfd);
    WSACleanup();
#else
	close(sockfd);
#endif
}

int main(int argc,char** argv)
{
	int i = 0;
#ifdef _WIN32
	pps_thread_t tid;	
#else
	pthread_t tid;	
#endif
	if(argc != 2)
	{
		printf("This program is used for finding hisi board's IP\n");
		printf("For example, if your hisi board's IP is 192.168.10.x\n");
		printf("then you can run this progorm as ./client 192.168.10\n");
		printf("please run this progorm in cmd/powershell/terminal\n");
		getchar();
		return -1;
	}
	strcpy(targetIP, argv[1]);
	for(i=1;i<=255;i++)
	{
#ifdef _WIN32
		pps_thread_create(&tid,NULL,run,(void *)i);
        pps_thread_detach(tid);
		Sleep(8);
#else
		pthread_create(&tid,NULL,run,(void *)i);
        pthread_detach(tid);
		usleep(8000);
#endif
        
	}
	while(1)
	{
		if(stop) 
		{
			printf("find over!  press any key to exit\n");
			getchar();
			return 0;
		}
	}
	return 0;
} 

```

# UDP广播方式

基本思想是客户端向255.255.255.255广播一条信息，服务器接收任意IP，接收到广播后，对客户端返回自己的IP，客户端设置接收超时时间，超过时间后，认为已经扫码完毕，退出程序

这种方式有个缺陷，不能跨网段，即便是物理上连接到一起，但是网段不同就没法通信

服务器和客户端的程序和之前TCP的差别不大，主要是变成c++程序了，编译器要选用g++

## 服务器

```cpp
//arm-hisiv500-linux-g++ server.c -o server -lpthread  
#include <iostream>
#include <strings.h>
#include <string.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
 
void trim(char *strIn, char *strOut);

int main()
{
    int sockfd;
    struct sockaddr_in saddr;
    int r;
    FILE *fp;
    char buf[1024];//缓冲区
    char recvline[1025];
    char sendline[1024] = {"Hello"};
    struct sockaddr_in presaddr;
    socklen_t len = sizeof(presaddr);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(8080);
    bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
    while (1)
    {
        r = recvfrom(sockfd, recvline, sizeof(recvline), 0 , (struct sockaddr*)&presaddr, &len);
        if (r <= 0)
        {
            perror("");
            exit(-1);
        }
        recvline[r] = 0;
        cout <<"recvfrom "<< inet_ntoa(presaddr.sin_addr) <<" " << recvline << endl;

        system("ifconfig -a | grep 'inet addr' > ipinfo");
        fp = fopen("ipinfo", "r");
        if (fp == NULL)
        {
            printf("fopen error!\n");
            break;
        }
        while (!feof(fp)) 
        { 
            fgets(buf,1024,fp);  //读取一行
            if(!feof(fp)) {
                trim(buf, sendline);
                printf("%s",sendline);
                sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&presaddr, sizeof(presaddr));
            }
        } 
        fclose(fp);
        system("rm ipinfo"); 
   
    }
    return 0;
}

void trim(char *strIn, char *strOut)
{
    int i, j ;
    i = 0;
    j = strlen(strIn) - 1;
    while(strIn[i] == ' ')
        ++i;
    while(strIn[j] == ' ')
        --j;
    strncpy(strOut, strIn + i , j - i + 1);
    strOut[j - i + 1] = '\0';
}
```

## 客户端

```cpp
//linux  g++ client.c -o client -lpthread
//win32  g++ client.c pps_thread_win32.c -o client.exe  -lwsock32
#include <iostream>
#ifdef _WIN32
	#include <stdio.h>
	#include <winsock2.h>
	#include "pps_thread.h"
	#pragma comment(lib,"ws2_32.lib")
#else
	#include <strings.h>
	#include <string.h>
	#include <sys/types.h>     
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <time.h>
#endif
using namespace std;

#define DEST_PORT 8080//目标地址端口号 

int stop = 0;

void *run(void *arg)
{
#ifdef _WIN32
	SOCKET sockfd;
	WSADATA wsaData;
	WORD wVersionRequired;
	wVersionRequired = MAKEWORD(2,2);
	if (WSAStartup(wVersionRequired, &wsaData)!= 0)
    {
        printf("init failed %ld\n",WSAGetLastError());
    }
    const char on = 1;
    int timeout = 1000; //1000ms
#else
	int sockfd;
	const int on = 1;
	struct timeval timeout;
	timeout.tv_sec=1; 	//1s
	timeout.tv_usec=0;
#endif
	int ret;
	char recvline[1024];
	char sendline[1024] = {"Hello"};
	
    struct sockaddr_in des_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)); 
#ifdef _WIN32
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //接收超时检测
#else
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); //接收超时检测
#endif

	memset(&des_addr, 0, sizeof(des_addr));
	des_addr.sin_family = AF_INET;
	des_addr.sin_addr.s_addr = inet_addr((char *)arg); //广播地址
	des_addr.sin_port = htons(DEST_PORT);
	
	sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&des_addr, sizeof(des_addr));
	

	while(1){
		memset(recvline, 0, sizeof(recvline));
		ret = recvfrom(sockfd, recvline, sizeof(recvline), 0, NULL, NULL);
		if(ret<=0) {
			stop = 1;
            break;
		}
		else
			cout << "server IP => " << recvline;	
	}
#ifdef _WIN32
	closesocket(sockfd);
    WSACleanup();
#else
	close(sockfd);
#endif
}


int main(int argc,char** argv)
{
	char targetIP[32];
#ifdef _WIN32
	pps_thread_t tid;	
#else
	pthread_t tid;	
#endif
	sprintf(targetIP, "255.255.255.255");
#ifdef _WIN32
    pthread_create(&tid,NULL,run,(void *)targetIP);
    pps_thread_detach(tid);
#else
    pthread_create(&tid,NULL,run,(void *)targetIP);
    pthread_detach(tid);
#endif
	while(1)
	{
		if(stop) 
		{
			printf("find over!  press any key to exit\n");
			getchar();
			return 0;
		}
	}
	return 0;
} 
```

# 重头戏：跨网段的P2P连接的UDP广播

这种实现方式，服务器和客户端分别用了2个端口，服务器向18081端口发送广播，客户端在18081端口接收数据，客户端向18080端口发送广播，服务器在18080端口接收数据，也就是说，客户端先向255.255.255.255广播，服务器收到信息后，判断出有人想得到IP信息，又向255.255.255.255广播，客户端只会广播一次，服务器也只会广播一次，避免造成广播风暴，使得网络瘫痪

其实从本质上说，双方即使服务器，又是客户端，只不过为了方便表述，把后台运行的叫做服务器

## 重要设置

在嵌入式机器上，广播不一定发的出去，sendto函数返回-1，perror显示network is unreachable，这种情况下，需要添加路由

* route add -net 255.255.255.255 netmask 255.255.255.255 dev eth0 metric 1
* route add -host 255.255.255.255 dev eth0

这两种方式均可，具体原理本渣渣还不清楚

## 服务器

```cpp
//arm-hisiv500-linux-g++ server.c -o server -lpthread  
#include <iostream>
#include <strings.h>
#include <string.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_SEND_PORT 18081
#define SERVER_RECV_PORT 18080

using namespace std;

void trim(char *strIn, char *strOut)
{
    int i, j ;
    i = 0;
    j = strlen(strIn) - 1;
    while(strIn[i] == ' ')
        ++i;
    while(strIn[j] == ' ')
        --j;
    strncpy(strOut, strIn + i , j - i + 1);
    strOut[j - i + 1] = '\0';
}

void *run(void *arg)
{
    int ret = 0;
    FILE *fp;
    char buf[1024];
    char sendline[1024] = "test";

    int send_sockfd;
    const int on = 1;
    struct sockaddr_in send_saddr;

    send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(send_sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)); 	//广播发送
    setsockopt(send_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bzero(&send_saddr, sizeof(send_saddr));
    send_saddr.sin_family = AF_INET;
    send_saddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    send_saddr.sin_port = htons(SERVER_SEND_PORT);

    system("ifconfig -a | grep 'inet addr' > ipinfo");
    fp = fopen("ipinfo", "r");
    if (fp == NULL)
    {
        printf("fopen error!\n");
        return NULL;
    }
    while (!feof(fp)) 
    {    
        fgets(buf, 1024, fp);  //读取一行
        if(!feof(fp)) {
            trim(buf, sendline);
            ret = sendto(send_sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&send_saddr, sizeof(send_saddr));
            cout << "read one line : " << sendline << endl;
            if(ret < 0)
		        cout << "send error!" << endl;
        }    
    } 
    fclose(fp);
    close(send_sockfd);
    system("rm ipinfo");
}

int main()
{
    int ret = 0;
    int recv_sockfd;
    struct sockaddr_in recv_saddr, client_saddr;
    socklen_t len = sizeof(client_saddr);
    char recvline[1025];
    pthread_t tid;
    
    recv_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&recv_saddr, sizeof(recv_saddr));
    recv_saddr.sin_family = AF_INET;
    recv_saddr.sin_addr.s_addr = htonl(INADDR_ANY);  //任意地址
    recv_saddr.sin_port = htons(SERVER_RECV_PORT);
    bind(recv_sockfd, (struct sockaddr*)&recv_saddr, sizeof(recv_saddr));
    
    while(1)
    {
        ret = recvfrom(recv_sockfd, recvline, sizeof(recvline), 0 , (struct sockaddr*)&client_saddr, &len);
        if(ret > 0)
        {
            recvline[ret] = '\0';
            cout <<"recvfrom "<< inet_ntoa(client_saddr.sin_addr) <<" " << recvline << endl;
            if(strcmp(recvline, "queryIP") == 0)
            {
                pthread_create(&tid, NULL, run, NULL);
                pthread_detach(tid);
            }
        }
    }
    close(recv_sockfd);
    return 0;
}
```



## 客户端

```cpp
//linux  g++ client.c -o client -lpthread
//win32  g++ client.c pps_thread_win32.c -o client.exe  -lwsock32
#include <iostream>
#ifdef _WIN32
	#include <stdio.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include "pps_thread.h"
	#pragma comment(lib,"ws2_32.lib")
#else
	#include <strings.h>
	#include <string.h>
	#include <sys/types.h>     
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <time.h>
#endif

using namespace std;

#define CLIENT_SEND_PORT 18080
#define CLIENT_RECV_PORT 18081

int main()
{
    int ret = 0;
#ifdef _WIN32
	SOCKET recv_sockfd, send_sockfd;
	WSADATA wsaData;
	WORD wVersionRequired;
	wVersionRequired = MAKEWORD(2,2);
	if (WSAStartup(wVersionRequired, &wsaData)!= 0)
    {
        printf("init failed %ld\n",WSAGetLastError());
    }
    const char on = 1;
	int timeout = 3000;
#else
    int recv_sockfd, send_sockfd;
    const int on = 1;
	struct timeval timeout;
	timeout.tv_sec=3; 
	timeout.tv_usec=0;
#endif

    struct sockaddr_in recv_saddr, client_saddr, send_saddr;
    socklen_t len = sizeof(client_saddr);
    char recvline[1025];
    char sendline[1024] = "queryIP";
  
    recv_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
	setsockopt(recv_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //接收超时检测
#else
	setsockopt(recv_sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); //接收超时检测
#endif
	setsockopt(recv_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&recv_saddr, 0, sizeof(recv_saddr));
    recv_saddr.sin_family = AF_INET;
    recv_saddr.sin_addr.s_addr = htonl(INADDR_ANY);  //任意地址
    recv_saddr.sin_port = htons(CLIENT_RECV_PORT);
    bind(recv_sockfd, (struct sockaddr*)&recv_saddr, sizeof(recv_saddr));

    send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(send_sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)); 	//广播发送
	setsockopt(send_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&send_saddr, 0, sizeof(send_saddr));
    send_saddr.sin_family = AF_INET;
    send_saddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    send_saddr.sin_port = htons(CLIENT_SEND_PORT);
	ret = sendto(send_sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&send_saddr, sizeof(send_saddr));
	if(ret < 0)
		cout << "send error!" << endl;

	while(1)
	{
		ret = recvfrom(recv_sockfd, recvline, sizeof(recvline), 0 , (struct sockaddr*)&client_saddr, &len);
		if(ret <= 0)
		{
			cout << "query end, press any key to exit" << endl;
#ifdef _WIN32
			getchar();
#endif
			break;
		}
		else
		{
			recvline[ret] = '\0';
			cout << "server IP => " << recvline;
		}
	}
#ifdef _WIN32
	closesocket(recv_sockfd);
	closesocket(send_sockfd);
    WSACleanup();
#else
	close(recv_sockfd);
	close(send_sockfd);
#endif	
	return 0;
}
```



