/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "ti_media_std.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <sii9233a_priv.h>

/* Global object storing all information related to all
  SII9233A driver handles */

/* Control API that gets called when Device_control is called

  This API does handle level semaphore locking

  handle - SII9233A driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Device_sii9233aControl (Device_Handle handle,
                              UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs)
{
    Int32 status;
    Device_Sii9233aObj *pObj = (Device_Sii9233aObj *)handle;

    if ( pObj == NULL )
        return -1;

    switch ( cmd )
    {
        case DEVICE_CMD_START:
            status = Device_sii9233aStart(pObj);
            break;

        case DEVICE_CMD_STOP:
            status = Device_sii9233aStop(pObj);
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_RESET:
            status = Device_sii9233aReset(pObj);
            break;
        
        case IOCTL_DEVICE_VIDEO_DECODER_GET_CHIP_ID:
            status = Device_sii9233aGetChipId(pObj,cmdArgs,cmdStatusArgs);
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_MODE:
            status = Device_sii9233aSetVideoMode(pObj, cmdArgs);
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS:
            status = Device_sii9233aGetVideoStatus ( pObj, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_EDID:
	    status =  Device_sii9233aSetupEDID(pObj);//add_by_cwj
            break;
			
        case IOCTL_DEVICE_VIDEO_DECODER_SET_INPUT_PORTTYPE: 
            status = Device_sii9233adetection(pObj);//add_by_cwj
           break;			
        default:
            status = -1;
            break;
    }

    return status;
}

/*
  Create API that gets called when Device_create is called

  This API does not configure the SII9233A is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  SII9233A

  drvId - driver ID, must be DEVICE_VID_DEC_SII9233A_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Device_Handle Device_sii9233aCreate (UInt8 drvId,UInt8 instId,
                                     Ptr createArgs,Ptr createStatusArgs)
{
    Int32 status;
    Device_Sii9233aObj *pObj;
    
    Device_VideoDecoderCreateParams *vidDecCreateArgs
        = (Device_VideoDecoderCreateParams *) createArgs;

    Device_VideoDecoderCreateStatus *vidDecCreateStatus
        = (Device_VideoDecoderCreateStatus *) createStatusArgs;

    if ( vidDecCreateArgs == NULL || vidDecCreateStatus == NULL )
        return NULL;

    vidDecCreateStatus->retVal = 0;

    if ( vidDecCreateArgs->deviceI2cInstId >= DEVICE_I2C_INST_ID_MAX
         || vidDecCreateArgs->numDevicesAtPort > DEVICE_SII9233A_DEV_MAX )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }

    pObj = (Device_Sii9233aObj *)malloc(sizeof(Device_Sii9233aObj));
    if ( pObj == NULL )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }
    else {
        memset(pObj, 0, sizeof(Device_Sii9233aObj));
        memcpy ( &pObj->createArgs, vidDecCreateArgs,
                 sizeof (*vidDecCreateArgs));

        status = OSA_i2cOpen(&pObj->i2cHandle, vidDecCreateArgs->deviceI2cInstId + 1);

        OSA_assert(status==0);
    }
    
    status = Device_sii9233aSetSlaveAddress(pObj);
    if ( status < 0 )
        return NULL;

    status = Device_sii9233aSetFactoryMode(pObj);
    if ( status < 0 )
        return NULL;

    status = Device_sii9233aSetupEDID(pObj);
    if ( status < 0 )
        return NULL;

    return pObj;
}

/*
  Delete function that is called when Device_delete is called

  This API
  - free's driver handle object

  handle - driver handle
  deleteArgs - NOT USED, set to NULL

*/
Int32 Device_sii9233aDelete (Device_Handle handle, Ptr deleteArgs )
{
    Device_Sii9233aObj *pObj = (Device_Sii9233aObj *) handle;

    if ( pObj == NULL )
        return -1;

    OSA_i2cClose(&pObj->i2cHandle);

    /*
     * free driver handle object
     */
    free(pObj);

    return 0;
}




