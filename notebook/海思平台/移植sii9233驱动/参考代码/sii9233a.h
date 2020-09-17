/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_SII9233A_H_
#define _DEVICEDRV_SII9233A_H_

Device_Handle Device_sii9233aCreate(UInt32 drvId,
                                    UInt32 instanceId,
                                    Ptr createArgs,
                                    Ptr createStatusArgs);

Int32 Device_sii9233aDelete(Device_Handle handle, Ptr deleteArgs);

Int32 Device_sii9233aControl(Device_Handle handle,
                             UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs);

#endif
