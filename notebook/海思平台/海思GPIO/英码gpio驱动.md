# 驱动的位置及加载

英码给了GPIO的驱动，在它的根文件系统中，系统启动时，加载该驱动即可

> /komod/extdrv/gpiobase.ko

驱动的加载，都是去修改 /komod/load3531d，这个文件原本是海思提供的，英码在里面加了自己的代码，修改下就好了

# 应用层

它这个代码还是很好理解的，就是自己换算一下pin-num就可以了

pinnum = gpiogroup * 8 + gpiopin

## gpio_test.c

```cpp
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ema_gpio.h"


int main(int argc , char* argv[])
{
	int fd = -1;
	TEmagpio tgpio;
	printf("/dev/emagpio\n");
	fd = open("/dev/emagpio", 0);
        
	if (fd<0)
	{
		printf("Open emagpio dev error!\n");
		return -1;
	}
	/* pinnum = gpiogroup * 8 + gpiopin*/
	tgpio.pinnum = 184; //gpio23_0
	tgpio.value = 1;
    
	ioctl(fd, GPIO_REQUEST, &tgpio); 
	ioctl(fd, GPIO_DIR_OUTPUT, &tgpio);
	while(1)
	{      
		tgpio.value = 1;
		ioctl(fd, GPIO_SET_VALUE, &tgpio);    
		sleep(1);
		tgpio.value = 0;
		ioctl(fd, GPIO_SET_VALUE, &tgpio);    
		sleep(1);
	 
	} 
	ioctl(fd, GPIO_FREE, &tgpio); 
    return 0;
}

```

## ema_gpio.h

```cpp
#ifndef __GPIO_BASE_DRV_H__
#define __GPIO_BASE_DRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


typedef struct
{
	unsigned int pinnum;
	unsigned char dir;
	unsigned char value;
} TEmagpio;

/************* gpio ioctl cmd************/
#define   EMA_GPIO_IOCTL_BASE    'E'

/*********  IOCTL CMD ***************/
#define    GPIO_DIR_INPUT     	_IOWR(EMA_GPIO_IOCTL_BASE, 0, TEmagpio)
#define    GPIO_DIR_OUTPUT     	_IOWR(EMA_GPIO_IOCTL_BASE, 1, TEmagpio)
#define    GPIO_GET_VALUE  		_IOWR(EMA_GPIO_IOCTL_BASE, 2, TEmagpio)
#define    GPIO_SET_VALUE     	_IOWR(EMA_GPIO_IOCTL_BASE, 4, TEmagpio)
#define    GPIO_REQUEST      	_IOWR(EMA_GPIO_IOCTL_BASE, 5, TEmagpio)
#define    GPIO_FREE     		_IOWR(EMA_GPIO_IOCTL_BASE, 6, TEmagpio)
/*********  IOCTL CMD END ***************/

/************* gpio ioctl cmd end ************/


/************* gpio kernel space api************/

int ema_gpio_is_valid(int number);

/* set as input or output, returning 0 or negative errno */
int ema_gpio_direction_input(unsigned int gpio);
int ema_gpio_direction_output(unsigned int gpio, int value);

/* GPIO INPUT: return zero or nonzero */
int ema_gpio_get_value(unsigned int gpio);

/* GPIO OUTPUT */
void ema_gpio_set_value(unsigned int gpio, int value);

int ema_gpio_cansleep(unsigned int gpio);


/* GPIO INPUT: return zero or nonzero, might sleep */
int ema_gpio_get_value_cansleep(unsigned int gpio);

/* GPIO OUTPUT, might sleep */
void ema_gpio_set_value_cansleep(unsigned int gpio, int value);


/* request GPIO, returning 0 or negative errno.
* non-null labels may be useful for diagnostics.
*/
int ema_gpio_request(unsigned int gpio, const char *label);

/* release previously-claimed GPIO */
void ema_gpio_free(unsigned int gpio);


/* map GPIO numbers to IRQ numbers */
int ema_gpio_to_irq(unsigned int gpio);

/* map IRQ numbers to GPIO numbers (avoid using this) */
//int ema_irq_to_gpio(unsigned irq);


int ema_get_irq_status(unsigned int gpio);


void ema_clear_irq_status(unsigned int gpio);



/* --------------------END ------------------------------*/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif	/* __GPIO_DRV_H__ */

```

