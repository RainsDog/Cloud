/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2020 Zigsun Technology Co.Ltd - http://www.zigsun.com/        *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "system.h"
#include "sii9233.h"

/* Structure declaration -----------------------------------------------------*/
typedef struct SII9233_DEV {
    int fd;                     //打开设备后的文件指针
    uint8_t dev_addr;
    uint8_t reg_addr[I2C_ONCE_WRITE_REGS_MAX];
    uint8_t reg_value[I2C_ONCE_WRITE_REGS_MAX];
}sii9233_dev_t;

/* Global variables ----------------------------------------------------------*/
sii9233_dev_t sii9233_dev;



int sii9233_open(sii9233_dev_t* dev) 
{
    if(dev == NULL)
    {
        printf("[error]sii9233_dev is NULL\n");
        return -1;
    }
    dev->fd = open("/dev/sii9233", O_RDWR);
    if(dev->fd < 0){
        printf("[error]open /dev/sii9233 error, ret=%d\n", dev->fd);
        return -1;
    }
    return 0;
}

int i2c_write8(int i2c_fd, uint16_t dev_addr,  uint8_t *reg, uint8_t *value, uint32_t reg_nums)
{
    int i,j;
    int status = 0;
    struct i2c_msg * msgs = NULL;
    struct i2c_rdwr_ioctl_data data;
    unsigned char * buf;

    //检查传入的参数
    if( (0 >= i2c_fd) || (0 == dev_addr) || (reg == NULL) || (value == NULL) || (reg_nums > I2C_ONCE_WRITE_REGS_MAX))
        goto ERROR;

    //为发送信息准备空间
    msgs = (struct i2c_msg *) malloc(sizeof(struct i2c_msg) * reg_nums);
    if(msgs==NULL)
        goto ERROR;
    buf = (unsigned char *) malloc(sizeof(unsigned char) * reg_nums * 2);
    if(buf == NULL)
        goto MALLOC_ERROR;
    
    //数据准备阶段
    for (i = 0, j = 0; i < reg_nums; i++, j+=2)
    {
        buf[j] = reg[i];
        buf[j + 1] = value[i];

        msgs[i].addr  = dev_addr;
        msgs[i].flags = 0;
        msgs[i].len   = 2;
        msgs[i].buf   = &buf[j];
    }
    data.msgs = msgs;
    data.nmsgs = reg_nums;

    //往I2C设备写数据
    status = ioctl(i2c_fd, I2C_RDWR, &data);
    if(status < 0)
        goto ERROR;

    free(msgs);
    free(buf);
    return 0;

MALLOC_ERROR:
    free(msgs);
ERROR:
    printf("[error] I2C write failed (addr=0x%02x, reg[0x%02x], reg_nums=%d)\n", dev_addr, reg[0], reg_nums);
    return -1;
}


//dev:0x60
//SLAVE_ADDR_4:0x15
//SLAVE_ADDR_2:0x18
//SLAVE_ADDR_3:0x19

//Programmable I2C device slave address for x.v.Color registers 
//Programmable I2C device slave address for CEC registers 
//Programmable I2C device slave address for EDID and NVRAM registers
/*
此函数的作用：
设置0x60地址上的3个寄存器的
    SLAVE_ADDR_4:0x15   值为0x64
    SLAVE_ADDR_2:0x18   值为0xc0
    SLAVE_ADDR_3:0x19   值为0xe0
这三个值是手册推荐的默认值，见p16
*/

int sii9233_set_slave_addr_register(sii9233_dev_t* dev)
{
    int ret = 0;
    uint8_t dev_addr = dev->dev_addr;
    uint8_t *reg_addr = dev->reg_addr;
    uint8_t *reg_value = dev->reg_value;

    reg_addr[0]  = DEVICE_SII9233A_REG_SLAVE_ADDRESS_4;
    reg_value[0] = 0x64;
    reg_addr[1]  = DEVICE_SII9233A_REG_SLAVE_ADDRESS_2;
    reg_value[1] = 0xc0; 
    reg_addr[2]  = DEVICE_SII9233A_REG_SLAVE_ADDRESS_3;
    reg_value[2] = 0xe0;
    ret = i2c_write8(dev->fd, dev_addr, reg_addr, reg_value, 3);
    if(ret < 0)
    {
        printf("[error] sii9233_set_slave_addr_register failed\n");
        return -1;
    }
    return 0;
}

/*
此函数的作用：
设置工厂寄存器的值
这一系列的寄存器都是有推荐值的，见p102
*/
int sii9233_set_factory_register(sii9233_dev_t* dev)
{
    int ret = 0;
    uint8_t dev_addr = dev->dev_addr;
    uint8_t *reg_addr = dev->reg_addr;
    uint8_t *reg_value = dev->reg_value;

    reg_addr[0] = 0x0e;
    reg_value[0] = 0x40;
    ret = i2c_write8(dev->fd, dev_addr, reg_addr, reg_value, 1);

    dev_addr = dev->dev_addr + DEVICE_SII9233A_I2C_EDID_OFFSET;  //0x70  *2 = 0xe0
    reg_addr[0] = 0xe5;
    reg_value[0] = 0x02;
    ret = i2c_write8(dev->fd, dev_addr, reg_addr, reg_value, 1);

    dev_addr = dev->dev_addr + DEVICE_SII9233A_I2C_XVCOLOR_OFFSET;   //0x32 * 2 = 0x64
    reg_addr[0] = 0x81;
    reg_value[0] = 0x18;
    reg_addr[1] = 0x87;
    reg_value[1] = 0x43;
    reg_addr[2] = 0x89;
    reg_value[2] = 0x00;
    reg_addr[3] = 0x92;
    reg_value[3] = 0x8a;
    reg_addr[4] = 0x93;
    reg_value[4] = 0xaa;
    reg_addr[5] = 0x94;
    reg_value[5] = 0x1a;
    reg_addr[6] = 0x95;
    reg_value[6] = 0x2a;
    reg_addr[7] = 0xb5;
    reg_value[7] = 0x40;
    reg_addr[8] = 0xbb;
    reg_value[8] = 0x04;
    ret = i2c_write8(dev->fd, dev_addr, reg_addr, reg_value, 9);
}


int sii9233_set_EDID_register(sii9233_dev_t* dev)
{
    
}



int sii9233_init(void)
{
    int ret = 0;
    memset(&sii9233_dev, 0, sizeof(sii9233_dev_t));
    sii9233_dev.dev_addr = 0x30;
    ret = sii9233_open(&sii9233_dev);
    if(ret < 0)
        goto ERROR;
    ret = sii9233_set_slave_addr_register(&sii9233_dev);
    if(ret < 0)
        goto ERROR;
    ret = sii9233_set_factory_register(&sii9233_dev);
    if(ret < 0)
        goto ERROR;
    ret = sii9233_set_EDID_register(&sii9233_dev);
    if(ret < 0)
        goto ERROR;
    return 0;

ERROR:
    printf("[error]sii9233_init error\n");
    return -1;
}

int sii9233_deinit(void)
{
    close(sii9233_dev.fd);
    return 0;
}
/*
int sii9233_control(int32_t cmd)
{
    int ret = 0;
    switch (cmd)
    {
        case DEVICE_CMD_START:
            ret = sii9233_start(pObj);
            break;
        case DEVICE_CMD_STOP:
            ret = sii9233_stop(pObj);
            break;
        case IOCTL_RESET:
            ret = sii9233_reset(pObj);
            break;
        case IOCTL_GET_CHIP_ID:
            ret = sii9233_get_chip_id(pObj,cmdArgs,cmdStatusArgs);
            break;
        case IOCTL_SET_VIDEO_MODE:
            ret = sii9233_set_video_mode(pObj, cmdArgs);
            break;
        case IOCTL_GET_VIDEO_STATUS:
            ret = sii9233_get_video_status ( pObj, cmdArgs, cmdStatusArgs);
            break;
        case IOCTL_SET_EDID:
	        ret = sii9233_set_EDID(pObj);
            break;
        case IOCTL_SET_INPUT_PORTTYPE: 
            ret = Device_sii9233adetection(pObj);
            break;			
        default:
            ret = -1;
            break;
    }
}
*/