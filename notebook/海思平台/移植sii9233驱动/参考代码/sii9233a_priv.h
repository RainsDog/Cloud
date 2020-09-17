/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICE_SII9233A_PRIV_H_
#define _DEVICE_SII9233A_PRIV_H_

#include <osa_sem.h>
#include <osa_i2c.h>

#include "ti_vcap_common_def.h"


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

/*
  Information that is parsed from HDMI AVIInfoFrame packet
*/
typedef struct
{
    UInt32 colorSpace;          /* RGB444 ot YUV422 or YUV422 */
    UInt32 colorImetric;        /* BT709 or BT601 */
    UInt32 pixelRepeat;         /* 1x, 2x, 4x */
    UInt32 detected;            /* detected or hard code*/

} Device_Sii9233aAviInfoFrame;

typedef struct
{
    Bool   isPalMode;
    /*palMode to be set based on detect status */
    Device_VideoDecoderVideoModeParams videoModeParams;
    /* video mode params */
    Device_VideoDecoderCreateParams createArgs;
    /* create time arguments */

    OSA_I2cHndl i2cHandle;
    /* i2cHandle for i2c read write operations */

} Device_Sii9233aObj;

Int32 Device_sii9233aStart(Device_Sii9233aObj *pObj);
Int32 Device_sii9233aStop(Device_Sii9233aObj *pObj);
Int32 Device_sii9233aReset(Device_Sii9233aObj * pObj );
Int32 Device_sii9233aGetChipId(Device_Sii9233aObj *pObj,
                               Device_VideoDecoderChipIdParams *pPrm,
                               Device_VideoDecoderChipIdStatus *pStatus);
Int32 Device_sii9233aSetVideoMode(Device_Sii9233aObj *pObj,
                                  Device_VideoDecoderVideoModeParams *pPrm);
Int32 Device_sii9233aGetVideoStatus(Device_Sii9233aObj *pObj,
                                    VCAP_VIDEO_SOURCE_STATUS_PARAMS_S *pPrm,
                                    VCAP_VIDEO_SOURCE_CH_STATUS_S *pStatus);
Int32 Device_sii9233aSetSlaveAddress(Device_Sii9233aObj *pObj);
Int32 Device_sii9233aSetFactoryMode(Device_Sii9233aObj *pObj);
Int32 Device_sii9233aSetupEDID(Device_Sii9233aObj *pObj);
#endif /*  _DEVICE_SII9233A_PRIV_H_  */
