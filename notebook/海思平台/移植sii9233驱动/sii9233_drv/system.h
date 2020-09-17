typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;


/* /dev/i2c-X ioctl commands.  The ioctl's parameter is always an
 * unsigned long, except for:
 *	- I2C_FUNCS, takes pointer to an unsigned long
 *	- I2C_RDWR, takes pointer to struct i2c_rdwr_ioctl_data
 *	- I2C_SMBUS, takes pointer to struct i2c_smbus_ioctl_data
 */
#define I2C_RETRIES	0x0701	/* number of times a device address should
				   be polled when not acknowledging */
#define I2C_TIMEOUT	0x0702	/* set timeout in units of 10 ms */

/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
 */
#define I2C_SLAVE	0x0703	/* Use this slave address */
#define I2C_SLAVE_FORCE	0x0706	/* Use this slave address, even if it
				   is already in use by a driver! */
#define I2C_TENBIT	0x0704	/* 0 for 7 bit addrs, != 0 for 10 bit */

#define I2C_FUNCS	0x0705	/* Get the adapter functionality mask */

#define I2C_RDWR	0x0707	/* Combined R/W transfer (one STOP only) */

#define I2C_PEC		0x0708	/* != 0 to use PEC with SMBus */
#define I2C_SMBUS	0x0720	/* SMBus transfer */
#define I2C_16BIT_REG   0x0709  /* 16BIT REG WIDTH */
#define I2C_16BIT_DATA  0x070a  /* 16BIT DATA WIDTH */
#define I2C_DMA     0x070b  /* DMA mode */

/* This is the structure as used in the I2C_SMBUS ioctl call */
struct i2c_smbus_ioctl_data {
	uint8_t read_write;
	uint8_t command;
	uint32_t size;
	union i2c_smbus_data  *data;
};

/* This is the structure as used in the I2C_RDWR ioctl call */
struct i2c_rdwr_ioctl_data {
	struct i2c_msg  *msgs;	/* pointers to i2c_msgs */
	uint32_t nmsgs;			/* number of i2c_msgs */
};

#define  I2C_RDRW_IOCTL_MAX_MSGS	42

struct i2c_msg {
	__u16 addr;	/* slave address			*/
	__u16 flags;
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_RD		0x0001	/* read data, from slave to master */
#define I2C_M_STOP		0x8000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
#define I2C_M_16BIT_REG		0x0002	/* indicate reg bit-width is 16bit */
#define I2C_M_16BIT_DATA	0x0008	/* indicate data bit-width is 16bit */
#define I2C_M_DMA			0x0004  /* indicate use dma mode */
	__u16 len;		/* msg length				*/
	__u8 *buf;		/* pointer to msg data			*/
};







#define DEVICE_SII9233A_DEV_MAX   (1)
/* Driver object state - NOT IN USE */
#define DEVICE_SII9233A_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define DEVICE_SII9233A_OBJ_STATE_IDLE     (1)

/* SII9233A I2C Port0 address is 0x30 or 0x31*/
/* SII9233A I2C Port1 offset from Port0  */
#define DEVICE_SII9233A_I2C_PORT1_OFFSET   (4)

/* SII9233A I2C XVCOLOR Port offset from Port0  */
#define DEVICE_SII9233A_I2C_XVCOLOR_OFFSET (2)

/* SII9233A I2C CEC Port offset from Port0  */
#define DEVICE_SII9233A_I2C_CEC_OFFSET     (0x30)

/* SII9233A I2C EDID Port offset from Port0  */
#define DEVICE_SII9233A_I2C_EDID_OFFSET    (0x40)

/* SII9233A Default TMDS Core ID to use  */
#define DEVICE_SII9233A_CORE_ID_DEFAULT    (0)

/* SII9233A Registers - I2C Port 0 */
#define DEVICE_SII9233A_REG_VND_IDL         (0x00)
#define DEVICE_SII9233A_REG_VND_IDH         (0x01)
#define DEVICE_SII9233A_REG_DEV_IDL         (0x02)
#define DEVICE_SII9233A_REG_DEV_IDH         (0x03)
#define DEVICE_SII9233A_REG_DEV_REV         (0x04)
#define DEVICE_SII9233A_REG_SW_RST_0        (0x05)
#define DEVICE_SII9233A_REG_STATE           (0x06)
#define DEVICE_SII9233A_REG_SW_RST_1        (0x07)
#define DEVICE_SII9233A_REG_SYS_CTRL_1      (0x08)
#define DEVICE_SII9233A_REG_SYS_SWTCHC      (0x09)
#define DEVICE_SII9233A_REG_HP_CTRL         (0x10)
#define DEVICE_SII9233A_REG_SLAVE_ADDRESS_4 (0x15)
#define DEVICE_SII9233A_REG_SLAVE_ADDRESS_2 (0x18)
#define DEVICE_SII9233A_REG_SLAVE_ADDRESS_3 (0x19)
#define DEVICE_SII9233A_REG_SYS_SWTCHC2     (0x0A)
#define DEVICE_SII9233A_REG_CHIP_SEL        (0x0C)
#define DEVICE_SII9233A_REG_HDCP_BCAPS_SET  (0x2E)
#define DEVICE_SII9233A_REG_H_RESL          (0x3A)
#define DEVICE_SII9233A_REG_H_RESH          (0x3B)
#define DEVICE_SII9233A_REG_V_RESL          (0x3C)
#define DEVICE_SII9233A_REG_V_RESH          (0x3D)
#define DEVICE_SII9233A_REG_VID_CTRL        (0x48)
#define DEVICE_SII9233A_REG_VID_MODE_2      (0x49)
#define DEVICE_SII9233A_REG_VID_MODE_1      (0x4A)
#define DEVICE_SII9233A_REG_VID_BLANK1      (0x4B)
#define DEVICE_SII9233A_REG_VID_BLANK2      (0x4C)
#define DEVICE_SII9233A_REG_VID_BLANK3      (0x4D)
#define DEVICE_SII9233A_REG_DE_PIXL         (0x4E)
#define DEVICE_SII9233A_REG_DE_PIXH         (0x4F)
#define DEVICE_SII9233A_REG_DE_LINL         (0x50)
#define DEVICE_SII9233A_REG_DE_LINH         (0x51)
#define DEVICE_SII9233A_REG_VID_STAT        (0x55)
#define DEVICE_SII9233A_REG_VID_CH_MAP      (0x56)
#define DEVICE_SII9233A_REG_VID_AOF         (0x5F)
#define DEVICE_SII9233A_REG_VID_XPM_EN      (0x6A)
#define DEVICE_SII9233A_REG_VID_XPCNTL      (0x6E)
#define DEVICE_SII9233A_REG_VID_XPCNTH      (0x6F)
#define DEVICE_SII9233A_REG_AEC0_CTRL       (0xB5)
#define DEVICE_SII9233A_REG_DS_BSTAT2       (0xD6)

/* SII9233A Registers - I2C Port 1 */
#define DEVICE_SII9233A_REG_SYS_I2S_CTRL2   (0x27)
#define DEVICE_SII9233A_REG_SYS_PWR_DWN_2   (0x3E)
#define DEVICE_SII9233A_REG_SYS_PWR_DWN     (0x3F)
#define DEVICE_SII9233A_REG_AVI_TYPE        (0x40)
#define DEVICE_SII9233A_REG_AVI_DBYTE15     (0x52)
#define DEVICE_SII9233A_REG_AUDP_STAT       (0x34)

#define DEVICE_SII9233A_EDID_ENABLE         (0x01)
#define DEVICE_SII9233A_EDID_FIFO_ADDR      (0x02)
#define DEVICE_SII9233A_EDID_FIFO_DATA      (0x03)
#define DEVICE_SII9233A_EDID_FIFO_SELECT    (0x04)

/* SII9233A Registers values */

/* PCLK stable, Clock detect, Sync Detect */
#define DEVICE_SII9233A_VID_DETECT          ((1<<0) | (1<<1) | (1<<4))

/* Video interlace status */
#define DEVICE_SII9233A_VID_INTERLACE       (1<<2)

/* color format */
#define DEVICE_SII9233A_COLOR_FORMAT_RGB    (0)
#define DEVICE_SII9233A_COLOR_FORMAT_YUV    (1)

/* F_xtal frequency in Khz  */
#define DEVICE_SII9233A_FXTAL_KHZ           (27000)

/* HDMI AVI InfoFrame Packet info  */

/* color space  */
#define DEVICE_SII9233A_AVI_INFO_COLOR_RGB444       (0)
#define DEVICE_SII9233A_AVI_INFO_COLOR_YUV422       (1)
#define DEVICE_SII9233A_AVI_INFO_COLOR_YUV444       (2)

/* color imetric  */
#define DEVICE_SII9233A_AVI_INFO_CMETRIC_NO_DATA    (0)
#define DEVICE_SII9233A_AVI_INFO_CMETRIC_ITU601     (1)
#define DEVICE_SII9233A_AVI_INFO_CMETRIC_ITU709     (2)

/* pixel repition */
#define DEVICE_SII9233A_AVI_INFO_PIXREP_NONE        (0)
#define DEVICE_SII9233A_AVI_INFO_PIXREP_2X          (1)
#define DEVICE_SII9233A_AVI_INFO_PIXREP_4X          (3)

/* AVI packet info values */
#define DEVICE_SII9233A_AVI_INFO_PACKET_CODE        (0x82)
#define DEVICE_SII9233A_AVI_INFO_VERSION_CODE       (0x02)
#define DEVICE_SII9233A_AVI_INFO_PACKET_LENGTH      (0x0D)