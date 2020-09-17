#include "ti_media_std.h"
#include "ti_vsys_common_def.h"
#include "mcfw/interfaces/link_api/system_const.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <sii9233a_priv.h>
#include <unistd.h>
//#define debug_act

#ifdef debug_act
  #define OSA_printf(...)  do { printf("\n\r [sii9233a] " __VA_ARGS__); fflush(stdout); } while(0)
#endif
//Device_Sii9233aObj gDevice_Sii9233aObj;
/*
  Enable or Disable output port
*/
Int32 Device_sii9233aOutputEnable (Device_Sii9233aObj *pObj, UInt32 enable)
{
    Int32 status = 0;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 devAddr;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0] + DEVICE_SII9233A_I2C_PORT1_OFFSET;

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SYS_PWR_DWN_2;
    regValue[numRegs] = 0xC3;
    if ( enable )
        regValue[numRegs] |= ( 1 << 2 );    /* Enable outputs  */
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_SYS_PWR_DWN;
    regValue[numRegs] = 0xAD;   /* Enable all expect outputs  */
    if ( enable )
        regValue[numRegs] |= ( 1 << 6 );    /* Enable outputs  */
    numRegs++;
        
    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    return status;
}

/*
  Power-ON All modules
*/
Int32 Device_sii9233aPowerDown(Device_Sii9233aObj *pObj, UInt32 powerDown)
{
    Int32 status = 0;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 devAddr;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0];

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SYS_CTRL_1;
    regValue[numRegs] = 0;
    numRegs++;
    
    status = OSA_i2cRead8 (&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0)
        return -1;

    if ( powerDown )
        regValue[0] &= 0xfd;  /* power down  */
    else
        regValue[0] |= 0x01;  /* normal operation */

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    return status;
}

/*
  Select TMDS core
  Ignore the input
*/
Int32 Device_sii9233aSelectTmdsCore(Device_Sii9233aObj *pObj, UInt32 coreId)
{
    Int32 status = 0;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 devAddr;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0];

   OSA_printf(" select_port = %d \n",coreId+1);

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SYS_SWTCHC;
    regValue[numRegs] = (0x01 << coreId) << 4;    /* DDC0_EN = 1 */
    numRegs++;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SYS_SWTCHC2;
    regValue[numRegs] = 0x08 | coreId;            /* DDCDLY_EN = 1, HDMI_PORT0 */
    numRegs++;
    regAddr[numRegs] = DEVICE_SII9233A_REG_HP_CTRL;
    regValue[numRegs] = 0x01 << (2 * coreId);     /* HPD0 = 1 */
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    return status;
}

Int32 Device_sii9233aSetSlaveAddress(Device_Sii9233aObj *pObj)
{
    Int32 status = 0;
    UInt8 regAddr[16];
    UInt8 regValue[16];
    UInt8 devAddr;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;
    
    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0];
    
    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SLAVE_ADDRESS_4;
    regValue[numRegs] = (devAddr + DEVICE_SII9233A_I2C_XVCOLOR_OFFSET) << 1;
    numRegs++;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SLAVE_ADDRESS_2;
    regValue[numRegs] = (devAddr + DEVICE_SII9233A_I2C_CEC_OFFSET) << 1;
    numRegs++;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SLAVE_ADDRESS_3;
    regValue[numRegs] = (devAddr + DEVICE_SII9233A_I2C_EDID_OFFSET) << 1;
    numRegs++;
    
    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    return status;
}

Int32 Device_sii9233aSetFactoryMode (Device_Sii9233aObj *pObj)
{
    Int32 status = 0;
    UInt8 regAddr[16];
    UInt8 regValue[16];
    UInt8 devAddr;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;
    
    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0];
    
    numRegs = 0;
    regAddr[numRegs] = 0x0e;
    regValue[numRegs] = 0x40;
    numRegs++;
    
    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    devAddr = pCreateArgs->deviceI2cAddr[0] + DEVICE_SII9233A_I2C_EDID_OFFSET;
    numRegs = 0;
    regAddr[numRegs] = 0xe5;
    regValue[numRegs] = 0x02;
    numRegs++;
    
    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    devAddr = pCreateArgs->deviceI2cAddr[0] + DEVICE_SII9233A_I2C_XVCOLOR_OFFSET;
    numRegs = 0;
    regAddr[numRegs] = 0x81;
    regValue[numRegs] = 0x18;
    numRegs++;
    regAddr[numRegs] = 0x87;
    regValue[numRegs] = 0x43;
    numRegs++;
    regAddr[numRegs] = 0x89;
    regValue[numRegs] = 0x00;
    numRegs++;
    regAddr[numRegs] = 0x92;
    regValue[numRegs] = 0x8a;
    numRegs++;
    regAddr[numRegs] = 0x93;
    regValue[numRegs] = 0xaa;
    numRegs++;
    regAddr[numRegs] = 0x94;
    regValue[numRegs] = 0x1a;
    numRegs++;
    regAddr[numRegs] = 0x95;
    regValue[numRegs] = 0x2a;
    numRegs++;
    regAddr[numRegs] = 0xb5;
    regValue[numRegs] = 0x40;
    numRegs++;
    regAddr[numRegs] = 0xbb;
    regValue[numRegs] = 0x04;
    numRegs++;
    
    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    return status;
}
 
Int32 Device_sii9233aSetupEDID(Device_Sii9233aObj *pObj)
{
    Int32 status = 0;
    UInt16 i;
    UInt8  regAddr[0x101];
    UInt8  regValue[0x101];
    UInt8  devAddr;
    UInt32  numRegs;
    UInt8 value,fifoValue;//add_by_cwj
    Device_VideoDecoderCreateParams *pCreateArgs;
#ifdef debug_act
   OSA_printf("enter  %s()! \n",__func__);
#endif   
    static UInt8 edid[0x100] = {
   #if 1  //demos
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4D, 0x29, 0x23, 0x92, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x12, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
	0x12, 0x48, 0x4C, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
	0x6E, 0x28, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x43,
	0x50, 0x39, 0x32, 0x32, 0x33, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x17, 0x78, 0x0F, 0x7E, 0x17, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x66,
	0x02, 0x03, 0x3D, 0x72, 0x55, 0x90, 0x04, 0x03, 0x02, 0x0E, 0x0F, 0x07, 0x23, 0x24, 0x05, 0x94,
	0x13, 0x12, 0x11, 0x1D, 0x1E, 0xA0, 0xA1, 0xA2, 0x01, 0x1F, 0x35, 0x09, 0x7F, 0x07, 0x09, 0x7F,
	0x07, 0x17, 0x07, 0x50, 0x3F, 0x06, 0xC0, 0x57, 0x06, 0x00, 0x5F, 0x7F, 0x01, 0x67, 0x1F, 0x00,
	0x83, 0x4F, 0x00, 0x00, 0x68, 0x03, 0x0C, 0x00, 0x10, 0x00, 0xB8, 0x2D, 0x00, 0x8C, 0x0A, 0xD0,
	0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x18, 0x8C,
	0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20, 0x0C, 0x40, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00,
	0x18, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00, 0xC4, 0x8E, 0x21,
	0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E
   #else//defult
        0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x16, 0x96, 0x68, 0x81, 0x01, 0x00, 0x00, 0x00,
        0x08, 0x17, 0x01, 0x03, 0x80, 0x10, 0x09, 0x78, 0x2a, 0x60, 0x41, 0xa6, 0x56, 0x4a, 0x9c, 0x25,
        0x12, 0x50, 0x54, 0x21, 0x08, 0x00, 0xd1, 0x00, 0xa9, 0x40, 0x81, 0x80, 0x81, 0x40, 0x81, 0x00,
        0x95, 0x00, 0xb3, 0x00, 0x01, 0x01, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c,
        0x45, 0x00, 0xa0, 0x5a, 0x00, 0x00, 0x00, 0x1e, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20,
        0x6e, 0x28, 0x55, 0x00, 0xa0, 0x5a, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x14,
        0x3c, 0x14, 0x51, 0x14, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfc,
        0x00, 0x45, 0x54, 0x56, 0x42, 0x4f, 0x41, 0x52, 0x44, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x01, 0x19,
        0x02, 0x03, 0x29, 0xf1, 0x4d, 0x10, 0x1f, 0x22, 0x21, 0x20, 0x05, 0x14, 0x04, 0x13, 0x12, 0x16,
        0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0
   #endif
    };

    value = Device_sii9233adetection(pObj);
 //   OSA_printf(" %s() : hdp_5v = 0x%x \n",__func__,value);

    if(value == 0x01)
		fifoValue =0x01;
    else if((value == 0x02) || (value == 0x03))
		fifoValue =0x02;
    else if((value == 0x04) || (value == 0x05))
		fifoValue =0x04;
    else if((value == 0x08) || (value == 0x09))
		fifoValue =0x08;
    else
		fifoValue =0x02;

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0] + DEVICE_SII9233A_I2C_EDID_OFFSET;

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_EDID_ENABLE;
    regValue[numRegs] = 0x0f;// defult:0x01
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_EDID_FIFO_SELECT;
    regValue[numRegs] = fifoValue; //  defult:0x01
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_EDID_FIFO_ADDR;
    regValue[numRegs] = 0x00;
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    numRegs = 0;
    regValue[numRegs] = DEVICE_SII9233A_EDID_FIFO_DATA;
    numRegs++;
    for (i=0; i<0x100; i++) {
        regValue[numRegs] = edid[i];
        numRegs++;
    }

    status = OSA_i2cRawWrite8(&pObj->i2cHandle, devAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
#ifdef debug_act
    numRegs = 0;
    regAddr[numRegs] = 0x01;
    numRegs++;
    regAddr[numRegs] = 0x02;
    numRegs++;
    regAddr[numRegs] = 0x03;
    numRegs++;
    regAddr[numRegs] = 0X04;
    numRegs++;
    status = OSA_i2cRead8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0)
        return -1;
    for(i=0;i<numRegs;i++)
    OSA_printf("%s() edid_set : regAddr[0x%x]=regValue[0x%x] !\n", __func__,regAddr[i], regValue[i]);
    OSA_printf("end  %s()! \n",__func__);
#endif
    return status;
}

Int32 Device_sii9233aSetEDID(Device_Sii9233aObj *pObj, Device_EdidDataParams *prm)
{
    Int32 status = 0;
    UInt16 i;
    UInt8  regAddr[0x101];
    UInt8  regValue[0x101];
    UInt8  devAddr;
    UInt32  numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;

#ifdef debug_act
OSA_printf("enter  %s()! \n",__func__);
#endif
    UInt8 * edid = prm->pedid;

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0] + DEVICE_SII9233A_I2C_EDID_OFFSET;

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_EDID_ENABLE;
    regValue[numRegs] = 0x0f;// defult:0x01
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_EDID_FIFO_SELECT;
    regValue[numRegs] = 0x02;// defult:0x01
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_EDID_FIFO_ADDR;
    regValue[numRegs] = 0x00;
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    numRegs = 0;
    regValue[numRegs] = DEVICE_SII9233A_EDID_FIFO_DATA;
    numRegs++;
    for (i = 0; i < prm->size; i++) {
        regValue[numRegs] = edid[i];
        numRegs++;
    }

    status = OSA_i2cRawWrite8(&pObj->i2cHandle, devAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
#ifdef debug_act
    numRegs = 0;
    regAddr[numRegs] = 0x01;
    numRegs++;
    regAddr[numRegs] = 0x02;
    numRegs++;
    regAddr[numRegs] = 0x03;
    numRegs++;
    regAddr[numRegs] = 0X04;
    numRegs++;
    status = OSA_i2cRead8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0)
        return -1;
    for(i=0;i<numRegs;i++)
    OSA_printf("%s() edid_set : regAddr[0x%x]=regValue[0x%x] !\n", __func__,regAddr[i], regValue[i]);
    OSA_printf("end  %s()! \n",__func__);
#endif
    return status;
}

/*
  Start output port
*/
Int32 Device_sii9233aStart(Device_Sii9233aObj *pObj)
{
    Int32 status = 0; 
#ifdef debug_act
    OSA_printf("enter  %s()! \n",__func__);
#endif
    status = Device_sii9233aOutputEnable(pObj,TRUE);

    return status;
}

/*
  Stop output port
*/
Int32 Device_sii9233aStop(Device_Sii9233aObj *pObj)
{
    Int32 status = 0;
#ifdef debug_act
    OSA_printf("enter  %s()! \n",__func__);
#endif
    status = Device_sii9233aOutputEnable(pObj,FALSE);

    return status;
}
Int32 Device_sii9233adetection(Device_Sii9233aObj *pObj)
{
    Int32 status = 0;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 devAddr;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0] + DEVICE_SII9233A_I2C_EDID_OFFSET;

    numRegs = 0;
    regAddr[numRegs] = 0xd1;
    numRegs++;
    status = OSA_i2cRead8 (&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0){
        return -1;
    }

   return regValue[0];
}

Int32 Device_sii9233aportswitch(Device_Sii9233aObj *pObj)
{
    Int32 status = 0;
    UInt8 value;

#ifdef debug_act
   OSA_printf("enter  %s()! \n",__func__);
#endif  
    value = Device_sii9233adetection(pObj);
    //OSA_printf("%s(): regAddr[0x%x]=regValue[0x%x] !\n", __func__,regAddr[0], regValue[0]);
    if(value == 1)
		status = Device_sii9233aSelectTmdsCore(pObj,0);
    else if( (value == 2) || (value == 3))
		status = Device_sii9233aSelectTmdsCore(pObj,1);
    else if( (value == 4) || (value == 5))
		status = Device_sii9233aSelectTmdsCore(pObj,2);
    else if( (value == 8)|| (value == 9))
		status = Device_sii9233aSelectTmdsCore(pObj,3);
    else
		status = Device_sii9233aSelectTmdsCore(pObj,1);

}

/*
  This API
  - Disable output
  - Power-ON of all module
  - Manual reset of SII9233A and then setup in auto reset mode
  - Select default TMDS core
*/
Int32 Device_sii9233aReset(Device_Sii9233aObj *pObj)
{
    Int32 status = 0;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 devAddr;
    UInt32 numRegs;
    UInt8 value;//add_by_cwj
#ifdef debug_act
   Int32 i;
#endif

#ifdef debug_act
OSA_printf("enter  %s()! \n",__func__);
#endif
    Device_VideoDecoderCreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0];
    
    /*
     * disable outputs
     */
    status = Device_sii9233aStop(pObj);
    if ( status < 0 )
        return status;

    /*
     * Normal operation
     */
    status = Device_sii9233aPowerDown(pObj,FALSE);
    if ( status < 0 )
        return status;

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SW_RST_0;
    regValue[numRegs] = 0x2F;   /* reset AAC, HDCP, ACR, audio FIFO, SW */
    numRegs++;
//add_by_cwj
    regAddr[numRegs] = DEVICE_SII9233A_REG_SW_RST_1;
    regValue[numRegs] = 0x09;   /* reset HDCP */ 
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    /*
     * wait for reset to be effective
     */
    usleep( 50000 );

    /*
     * remove reset and setup in auto-reset mode
     */
    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_SW_RST_0;
    regValue[numRegs] = 0x10;   /* auto-reset SW */
    numRegs++;
    
    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    /*
     * select TDMS core
     */
    Device_sii9233aportswitch(pObj);

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_HDCP_BCAPS_SET;
    regValue[numRegs] = 0xE0;   /* set HDCP */               //debug_cwj
    numRegs++;
    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);

#ifdef debug_act
    numRegs = 0;
    regAddr[numRegs] = 0x1A;
    numRegs++;
    regAddr[numRegs] = 0x1B;
    numRegs++;
    regAddr[numRegs] = 0x1C;
    numRegs++;
    regAddr[numRegs] = 0X1D;
    numRegs++;
    regAddr[numRegs] = 0x1E;
    numRegs++;
    regAddr[numRegs] = DEVICE_SII9233A_REG_HDCP_BCAPS_SET;
    numRegs++;
    status = OSA_i2cRead8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0)
        return -1;
    for(i=0;i<numRegs;i++)
	OSA_printf("hdcp_ksv_val : regAddr[0x%x]=regValue[0x%x] !\n", regAddr[i], regValue[i]);

#endif

    return status;
}

/*
  Get Chip ID and revision ID
*/
Int32 Device_sii9233aGetChipId(Device_Sii9233aObj *pObj,
                               Device_VideoDecoderChipIdParams *pPrm,
                               Device_VideoDecoderChipIdStatus *pStatus)
{
    Int32 status = 0;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 devAddr;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pStatus == NULL || pPrm == NULL )
        return -1;

    memset ( pStatus, 0, sizeof ( *pStatus ) );

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0];

    if ( pPrm->deviceNum >= pCreateArgs->numDevicesAtPort )
        return -1;

    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_VND_IDL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_VND_IDH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_DEV_IDL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_DEV_IDH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_DEV_REV;
    regValue[numRegs] = 0;
    numRegs++;

    status = OSA_i2cRead8 (&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0)
        return -1;

    pStatus->chipId = ( ( UInt32 ) regValue[3] << 8 ) | regValue[2];
    pStatus->chipRevision = regValue[4];
    pStatus->firmwareVersion = ( ( UInt32 ) regValue[1] << 8 ) | regValue[0];

    return status;
}

/*
  Read AVI packet info and parse information from it

  pAviInfo - parsed information returned by this API
*/
Int32 Device_sii9233aReadAviInfo(Device_Sii9233aObj *pObj,
                                 Device_Sii9233aAviInfoFrame *pAviInfo)
{
    Int32 status = 0;
    UInt8 regAddr[32];
    UInt8 regValue[32];
    UInt8 devAddr;
    UInt8 aviAddr;
    UInt8 *aviData;
    UInt32 numRegs;
    Device_VideoDecoderCreateParams *pCreateArgs;

    /*
     * read AVI Info Frame
     */
    pAviInfo->detected = FALSE;
    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0] + DEVICE_SII9233A_I2C_PORT1_OFFSET;

    numRegs = 0;

    for ( aviAddr = DEVICE_SII9233A_REG_AVI_TYPE;
          aviAddr <= DEVICE_SII9233A_REG_AVI_DBYTE15; aviAddr++ )
    {
        regAddr[numRegs] = aviAddr;
        regValue[numRegs] = 0;
        numRegs++;
    }

    status = OSA_i2cRead8 (&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0){
        return -1;
    }

    if ( regValue[0] == DEVICE_SII9233A_AVI_INFO_PACKET_CODE
         && regValue[1] == DEVICE_SII9233A_AVI_INFO_VERSION_CODE
         && regValue[2] == DEVICE_SII9233A_AVI_INFO_PACKET_LENGTH )
    {
        /*
         * Valid AVI packet recevied
         */

        status = 0;
        pAviInfo->detected = TRUE;

        aviData = &regValue[3]; /* point to start of AVI data checksum */

        /*
         * parse information
         */
        pAviInfo->colorSpace = ( ( aviData[1] >> 5 ) & 0x3 );
        pAviInfo->colorImetric = ( ( aviData[2] >> 6 ) & 0x3 );
        pAviInfo->pixelRepeat = ( ( aviData[5] >> 0 ) & 0xF );
    }

    return status;
}

/*
  Get and detect video status
*/
Int32 Device_sii9233aGetVideoStatus(Device_Sii9233aObj *pObj,
                                    VCAP_VIDEO_SOURCE_STATUS_PARAMS_S *pPrm,
                                    VCAP_VIDEO_SOURCE_CH_STATUS_S *pStatus)
{
    Int32 status = 0;
    UInt8 regAddr[16];
    UInt8 regValue[16];
    UInt8 devAddr;
    UInt32 numRegs;
    UInt32 hRes, vRes, dePix, deLin, xclkInPclk;
    UInt32 sysStat, vidStat;
    UInt32 t_line;
    Device_VideoDecoderCreateParams *pCreateArgs;
#ifdef debug_act
OSA_printf("enter  %s()! \n",__func__);
#endif    
    if ( pStatus == NULL || pPrm == NULL )
        return -1;

    memset ( pStatus, 0, sizeof ( *pStatus ) );

    pCreateArgs = &pObj->createArgs;
    devAddr = pCreateArgs->deviceI2cAddr[0];

    // stop count
    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_XPM_EN;
    regValue[numRegs] = 0x00;
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;
    
    numRegs = 0;

    regAddr[numRegs] = DEVICE_SII9233A_REG_H_RESL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_H_RESH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_V_RESL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_V_RESH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_DE_PIXL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_DE_PIXH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_DE_LINL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_DE_LINH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_XPCNTL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_XPCNTH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_STAT;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_STATE;
    regValue[numRegs] = 0;
    numRegs++;
    
    status = OSA_i2cRead8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0)
        return -1;
    
    /*
     * horizontal resolution
     */
    hRes = ( ( UInt32 ) regValue[1] << 8 ) | regValue[0];

    /*
     * vertical resolution
     */
    vRes = ( ( UInt32 ) regValue[3] << 8 ) | regValue[2];

    /*
     * horizontal active data resolution
     */
    dePix = ( ( UInt32 ) regValue[5] << 8 ) | regValue[4];

    /*
     * vertical active data resolution
     */
    deLin = ( ( UInt32 ) regValue[7] << 8 ) | regValue[6];

    /*
     * number of xclks per 2048 video clocks
     */
    xclkInPclk = ( ( UInt32 ) regValue[9] << 8 ) | regValue[8];

    /*
     * video status
     */
    vidStat = regValue[10];

    /*
     * system status
     */
    sysStat = regValue[11];

    if ( sysStat & DEVICE_SII9233A_VID_DETECT )
    {
        pStatus->isVideoDetect = TRUE;

        if ( vidStat & DEVICE_SII9233A_VID_INTERLACE )
        {
             pStatus->isInterlaced = TRUE;
        }

        pStatus->frameWidth = dePix;
        pStatus->frameHeight = deLin;

        /*
         * time interval in usecs for each line
         */
        t_line = ( UInt32 ) ( ( ( UInt32 ) hRes * xclkInPclk * 1000 ) / ( DEVICE_SII9233A_FXTAL_KHZ * 2048 ) ); /* in usecs */

        /*
         * time interval in usecs for each frame/field
         */
        pStatus->frameInterval = t_line * vRes;
    }

    //enable
    numRegs = 0;
    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_XPM_EN;
    regValue[numRegs] = 0x01;
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    return status;
}

/*
  Wait until video is detected and the get video properties
*/
Int32 Device_sii9233aDetectVideo(Device_Sii9233aObj *pObj,
                                 Device_Sii9233aAviInfoFrame *pAviInfo,
                                 UInt32 timeout)
{
    Int32 status = 0;
    VCAP_VIDEO_SOURCE_STATUS_PARAMS_S vidStatusPrm;
    VCAP_VIDEO_SOURCE_CH_STATUS_S vidStatus;
    UInt32 loopTimeout, curTimeout = 0;

    timeout = 1000; /* hard code for now */
    loopTimeout = 250;   /* check video status every 50 OS ticks */

    usleep(100000);
    vidStatusPrm.channelNum = 0;
    while ( 1 )
    {
        status = Device_sii9233aGetVideoStatus (pObj, &vidStatusPrm, &vidStatus);
        if ( status < 0)
            return status;

        if ( vidStatus.isVideoDetect )
        {
            break;
        }
        usleep ( loopTimeout * 1000);

        if ( timeout != 0xFF )
        {
            curTimeout += loopTimeout;

            if ( curTimeout >= timeout )
            {
                status = -1;
                break;
            }
        }
    }

    /*
     * video detected, read AVI info
     */
    status |= Device_sii9233aReadAviInfo ( pObj, pAviInfo );

    return status;
}

/*
  Setup video processing path based on detected source

  pAviInfoFrame - input source properties
  pPrm - user required parameters
*/
Int32 Device_sii9233aSetupVideo(Device_Sii9233aObj * pObj,
                                Device_Sii9233aAviInfoFrame * pAviInfo,
                                Device_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = 0;
    UInt8 regAddr[32];
    UInt8 regValue[32];
    UInt8 devAddr;
    UInt32 numRegs;
    UInt32 insSavEav, cscR2Y, upSmp, downSmp, chMap;
    UInt32 cscY2R, outColorSpace, isBT709, inPixRep;
    Device_VideoDecoderCreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];
#ifdef debug_act
OSA_printf("enter  %s()! \n",__func__);
#endif
    outColorSpace = DEVICE_SII9233A_COLOR_FORMAT_YUV;
    if ( pPrm->videoDataFormat == SYSTEM_DF_RGB24_888 )
        outColorSpace = DEVICE_SII9233A_COLOR_FORMAT_RGB;
    // Hard code AVI frame info
    if ( pAviInfo->detected == FALSE)
    {
    	pAviInfo->colorSpace = DEVICE_SII9233A_AVI_INFO_COLOR_RGB444;
    	pAviInfo->colorImetric = DEVICE_SII9233A_AVI_INFO_CMETRIC_ITU709;
    	pAviInfo->pixelRepeat = DEVICE_SII9233A_AVI_INFO_PIXREP_NONE;

        if ( pPrm->standard==SYSTEM_STD_480I
          || pPrm->standard==SYSTEM_STD_576I
          || pPrm->standard==SYSTEM_STD_D1
          || pPrm->standard==SYSTEM_STD_NTSC
          || pPrm->standard==SYSTEM_STD_PAL )
        {
            pAviInfo->colorImetric = DEVICE_SII9233A_AVI_INFO_CMETRIC_ITU601;
            pAviInfo->pixelRepeat = DEVICE_SII9233A_AVI_INFO_PIXREP_2X;
        }
    }

    inPixRep = pAviInfo->pixelRepeat;
    if ( inPixRep > DEVICE_SII9233A_AVI_INFO_PIXREP_4X )
    {
        inPixRep = 0;
        status = -1;
    }
    
    isBT709 = 1;
    if ( pAviInfo->colorImetric == DEVICE_SII9233A_AVI_INFO_CMETRIC_ITU601 )
        isBT709 = 0;

    cscR2Y = 0;
    upSmp = 0;
    downSmp = 0;
    cscY2R = 0;
    insSavEav = 0;

    switch ( pAviInfo->colorSpace )
    {
        case DEVICE_SII9233A_AVI_INFO_COLOR_RGB444:
            switch ( pPrm->videoDataFormat )
            {
                case SYSTEM_DF_YUV422P:
                    cscR2Y = 1;
                    downSmp = 1;
                    insSavEav = 1;
                    break;

                case SYSTEM_DF_YUV444P:
                    cscR2Y = 1;
                    break;

                case SYSTEM_DF_RGB24_888:
                    break;

                default:
                    status = -1;
                    break;
            }
            break;
        case DEVICE_SII9233A_AVI_INFO_COLOR_YUV444:
            switch ( pPrm->videoDataFormat )
            {
                case SYSTEM_DF_YUV422P:
                    downSmp = 1;
                    insSavEav = 1;
                    break;

                case SYSTEM_DF_YUV444P:
                    break;

                case SYSTEM_DF_RGB24_888:
                    cscY2R = 1;
                    break;

                default:
                    status = -1;
                    break;
            }
            break;

        case DEVICE_SII9233A_AVI_INFO_COLOR_YUV422:
            switch ( pPrm->videoDataFormat )
            {
                case SYSTEM_DF_YUV422P:
                    insSavEav = 1;
                    break;

                case SYSTEM_DF_YUV444P:
                    upSmp = 1;
                    break;

                case SYSTEM_DF_RGB24_888:
                    upSmp = 1;
                    cscY2R = 1;
                    break;

                default:
                    status = -1;
                    break;
            }
            break;

        default:
            status = -1;
            break;
    }
    numRegs = 0;

    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_MODE_1;
    regValue[numRegs] = ( insSavEav << 7 )  /* 1: SAV2EAV enable   , 0: disable */
        | ( 0 << 6 )    /* 1: Mux Y/C          , 0: No MUX  */
        | ( 1 << 5 )    /* 1: Dither enable    , 0: disable */
        | ( 1 << 4 )    /* 1: R2Y compress     , 0: bypass  */
        | ( cscR2Y << 3 )   /* 1: Enable R2Y CSC   , 0: bypass  */
        | ( upSmp << 2 )    /* 1: YUV422 to YUV444 , 0: bypass  */
        | ( downSmp << 1 )  /* 1: YUV444 to YUV422 , 0: bypass  */
        ;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_MODE_2;
    regValue[numRegs] = ( 0 << 6 )  /* 0: Dither to 8bits, 1: 10bits, 2: 12bits  */
        | ( 0 << 5 )    /* 0: EVNODD LOW if field 0 is even, 1: HIGH */
        | ( 0 << 3 )    /* 1: Y2R compress     , 0: bypass , modify by ltz 20161027, adjust color difference, last value is 1 */
        | ( cscY2R << 2 )   /* 1: Y2R CSC          , 0: bypass           */
        | ( outColorSpace << 1 )    /* 0: Output format RGB, 1: YUV           */
        | ( 0 << 0 )    /* 1: Range clip enable, 0: disable          */
        ;
    numRegs++;

    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_CTRL;
    regValue[numRegs] = ( 0 << 7 )  /* 0: Do not invert VSYNC, 1: invert */
        | ( 0 << 6 )    /* 0: Do not invert HSYNC, 1: invert */
        | ( isBT709 << 2 )  /* 0: Y2R BT601          , 1: BT709  */
        | ( 0 << 1 )    /* 0: 8bits RGB or YUV   , 1: YUV422 > 8 bits */
        | ( isBT709 << 0 )  /* 0: R2Y BT601          , 1: BT709  */
        ;
    numRegs++;
    
    regAddr[numRegs] = DEVICE_SII9233A_REG_SYS_CTRL_1;
    regValue[numRegs] = ( inPixRep << 6 )  /* 0: Output pixel clock divided 1x, 1: 1/2x, 3: 1/4x */
        | ( inPixRep << 4 ) /* 0: Input pixel replicate 1x, 1:2x, 3:4x            */
        | ( 1 << 2 )    /* 0: 12-bit mode         , 1: 24-bit mode      */
        | ( 0 << 1 )    /* 0: Normal output clock , 1: Invert clock     */
        | ( 1 << 0 )    /* 0: Power down          , 1: Normal operation */
        ;
    numRegs++;

    /* this is device connected to VIP1 and is connected in
       16-bit mode */
    chMap = 0x00;

    /*     Q[23:16]  Q[15:8]  Q[7:0]
        0:     R        G       B
        1:     R        B       G
        2:     G        R       B
        3:     G        B       R
        4:     B        R       G
        5:     B        G       R
    */
    regAddr[numRegs] = DEVICE_SII9233A_REG_VID_CH_MAP;
    regValue[numRegs] = chMap;
    numRegs++;

    status = OSA_i2cWrite8(&pObj->i2cHandle, devAddr, regAddr, regValue, numRegs);
    if ( status < 0 )
        return -1;

    return status;
}
 
/*
  For SII9233A below parameters in Device_VideoDecoderVideoModeParams are ignored
  and any value set by user for these parameters is not effective.

  videoIfMode
  standard
  videoCaptureMode
  videoSystem

  Depending on video data format SII9233A is configured as below

  videoDataFormat =
   SYSTEM_DF_YUV422P   : 16-bit YUV422 single CH embedded sync auto-detect mode
   SYSTEM_DF_YUV444P   : 24-bit YUV444 single CH discrete sync auto-detect mode
   SYSTEM_DF_RGB24_888 : 24-bit RGB24  single CH discrete sync auto-detect mode
*/
Int32 Device_sii9233aSetVideoMode(Device_Sii9233aObj *pObj,
                                  Device_VideoDecoderVideoModeParams *pPrm)
{
    Int32 status = 0;
    Device_Sii9233aAviInfoFrame aviInfo;
#ifdef debug_act
OSA_printf("enter  %s()! \n",__func__);
#endif
    /*
     * reset device
     */
    status = Device_sii9233aReset(pObj);
    if ( status < 0 )
        return status;

    /*
     * detect video source properties
     */
    status = Device_sii9233aDetectVideo(pObj,
                                        &aviInfo, pPrm->videoAutoDetectTimeout);
    /*
     * setup video processing path based on detected source
     */
    status = Device_sii9233aSetupVideo ( pObj, &aviInfo, pPrm );
    if ( status < 0 )
        return status;

    return status;
}


