# IOCTL 命令说明

所有的ioctl都需要传一个 联合体 Audio_Ctrl 指针类型的参数，此联合体及其中结构体的定义在tlv320aic32x4.h中

```cpp
typedef union
{
    struct audio_interface_t interface;
    struct other_config_t other;
    struct adc_dac_config_t adc;
    struct adc_dac_config_t dac;
    struct agc_config1_t agc_conf1;
    struct agc_config2_t agc_conf2;
    struct agc_config3_t agc_conf3;
    struct agc_config4_t agc_conf4;
    struct audio_power_up_t powerup;
    struct hp_route_t hp;
    struct lo_route_t lo;
    struct output_volume_t output_volume;
    struct output_mute_t output_mute;
    struct micpga_left_t micpga_left;
    struct micpga_right_t micpga_right;
    struct micpga_volum_t micpga_volume;
    struct float_input_t float_input;
}Audio_Ctrl
```

例如以下指令用来将adc静音

```cpp
Audio_Ctrl audio_ctrl;

audio_ctrl.adc.mute = 1;

ioctl(pfd, ADC_MUTE, &audio_ctrl);
```

根据cmd命令的类型，选择对Audio_Ctrl的成员进行赋值，部分cmd命令执行并不依赖所传递的参数，但是Audio_Ctrl参数还是要给，不能直接给NULL

下文针对每一个cmd给出使用示例，示例中将对联合体的部分成员进行赋值，这部分成员是必须赋值的，不能省略不写，因为联合体的原因，即使不写，也会保留上一次的赋值，传到驱动层后可能因为参数错误而不会执行对应的命令

因为codec分左右声道，可以分开设置，但是为了方便，也提供了统一设置的命令，如设置DAC播放的音量，可以选择DAC_VOLUME命令对左右声道统一设置，也可以使用DAC_LEFT_VOLUME和DAC_RIGHT_VOLUME命令分开设置，总之选择一种即可，本驱动中有大量类似的命令，使用时需要注意



## SOFT_RESET

* 复位所有寄存器，并按默认值进行初始化（codec i2s从模式，采样率48k，位深16bit）
* 完成初始化需要一定的时间，这会导致应用程序处于阻塞状态
* 初始化完成后，即可正常工作，一般情况下，除了音量的调节和静音控制，不需要进行其它ioctl操作
* 不能将此reset认为是单纯的重置所有寄存器，因为它还使用默认参数做了很多初始化工作
* 参数个数：0
* 使用示例：

```cpp
ioctl(s_fdTlv,SOFT_RESET,&audio_ctrl)
```

## LIST_REG_VALUE
* 打印出页1和页2的所有寄存器值
* 此命令一般用于调试
* 参数个数：0

* 使用示例：

```cpp
ioctl(s_fdTlv,LIST_REG_VALUE,&audio_ctrl);
```

## TRANSFER_MODE

* 设置音频数据传输类型，如I2S、PCM，目前只支持I2S
* 当配置与默认配置不同时，需要调用该命令进行修改，目前默认配置为I2S
* 参数个数：1
* 使用示例：

```cpp
audio_ctrl.interface.transfer_mode = AIC32x4_TRANSFER_MODE_I2S;
ioctl(s_fdTlv,TRANSFER_MODE, &audio_ctrl)
```

## BIT_WIDTH

* 设置音频位深（采样精度），如8bit、16bit、24bit，目前只支持16bit
* 当配置与默认配置不同时，需要调用该命令进行修改，目前默认配置为16bit
* 参数个数：2
* 注意事项：需要传递AIO_ATTR_S结构体中的u32ChnCnt成员作为通道数量，此变量在codec处于主模式下起作用，用于计算BCLK分频系数

* 使用示例：

```cpp
audio_ctrl.interface.bit_width = AIC32x4_BIT_WIDTH_16B;
audio_ctrl.interface.chn_num = pstAioAttr->u32ChnCnt;
ioctl(s_fdTlv,BIT_WIDTH, &audio_ctrl)
```

## MASTER_SLAVE_MODE

* 设置codec的主从模式，注意不是海思的，例如当海思为主时，需要设置codec为从
* 当配置与默认配置不同时，需要调用该命令进行修改，目前默认配置为海思主codec从

* 传递参数个数：1

* 使用示例： 

```cpp
audio_ctrl.interface.master_slave_mode = AIC32x4_SLAVE_MODE;
ioctl(s_fdTlv,MASTER_SLAVE_MODE, &audio_ctrl)
```

## SAMPLE_RATE

* 设置采样率（采样频率）
* 当配置与默认配置不同时，需要调用该命令进行修改，目前默认配置为48K

* 传递参数个数：1

* 使用示例： 

```cpp
audio_ctrl.interface.sample_rate = AIC32x4_SAMPLE_RATE_48K;
ioctl(s_fdTlv,SAMPLE_RATE, &audio_ctrl)
```

## DATA_OFFSET

* 设置数据在BCLK时钟上的偏移量
* **此配置目前不需要修改，默认为0**
* 参数个数：1
* 参数取值范围：0-255
* 使用示例： 

```cpp
audio_ctrl.interface.data_offset = 0;
ioctl(s_fdTlv,DATA_OFFSET, &audio_ctrl)
```

## LOOPBACK_TEST1

* 环出设置，data in 环出到 data out，不经过adc、dac、mixer等
* 仅用于测试
* 默认不使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.other.loop_data_in_to_data_out = AIC32x4_DISABLE; 
ioctl(s_fdTlv,LOOPBACK_TEST1, &audio_ctrl)
```

## LOOPBACK_TEST2

* 环出设置，adc输出 环出到 dac输入
* 仅用于测试
* 默认不使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.other.loop_adc_to_dac = AIC32x4_DISABLE;
ioctl(s_fdTlv,LOOPBACK_TEST2, &audio_ctrl)
```

## ADC_POWER_UP

* ADC采集左右声道使能
* 默认已使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.adc.power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,ADC_POWER_UP, &audio_ctrl)
```

## ADC_LEFT_POWER_UP

* ADC采集左声道使能
* 默认已使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.adc.left_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,ADC_LEFT_POWER_UP, &audio_ctrl)
```

## ADC_RIGHT_POWER_UP

* ADC采集右声道使能
* 默认已使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.adc.right_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,ADC_RIGHT_POWER_UP, &audio_ctrl)
```

## ADC_MUTE

* ADC采集左右声道静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.adc.mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,ADC_MUTE, &audio_ctrl)
```

## ADC_LEFT_MUTE

* ADC采集左声道静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.adc.left_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,ADC_LEFT_MUTE, &audio_ctrl)
```

## ADC_RIGHT_MUTE

* ADC采集右声道静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.adc.right_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,ADC_RIGHT_MUTE, &audio_ctrl)
```

## ADC_VOLUME

* ADC左右声道音量增益
* 默认增益0dB
* 传递参数个数：1
* 参数范围：0-64（-12dB ~ 20dB）
* 使用示例： 

```cpp
audio_ctrl.adc.volume = AIC32x4_ADC_VOLUME_DEFAULT;
ioctl(s_fdTlv,ADC_VOLUME, &audio_ctrl)
```

## ADC_LEFT_VOLUME

* ADC左声道音量增益
* 默认增益0dB
* 传递参数个数：1
* 参数范围：0-64（-12dB ~ 20dB）
* 使用示例： 

```cpp
audio_ctrl.adc.left_volume = AIC32x4_ADC_VOLUME_DEFAULT;
ioctl(s_fdTlv,ADC_LEFT_VOLUME, &audio_ctrl)
```

## ADC_RIGHT_VOLUME

* ADC右声道音量增益
* 默认增益0dB
* 传递参数个数：1
* 参数范围：0-64（-12dB ~ 20dB）
* 使用示例： 

```cpp
audio_ctrl.adc.right_volume = AIC32x4_ADC_VOLUME_DEFAULT;
ioctl(s_fdTlv,ADC_RIGHT_VOLUME, &audio_ctrl)
```

## DAC_POWER_UP

* DAC播放左右声道使能
* 默认已使能
* 传递参数个数：1
* 使用示例： 

```cpp
udio_ctrl.dac.power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,DAC_POWER_UP, &audio_ctrl)
```

## DAC_LEFT_POWER_UP

* DAC播放左声道使能
* 默认已使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.dac.left_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,DAC_LEFT_POWER_UP, &audio_ctrl)
```

## DAC_RIGHT_POWER_UP

* DAC播放右声道使能
* 默认已使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.dac.right_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,DAC_RIGHT_POWER_UP, &audio_ctrl)
```

## DAC_MUTE

* DAC播放左右声道静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.dac.mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,DAC_MUTE, &audio_ctrl)
```

## DAC_LEFT_MUTE

* DAC播放左声道静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.dac.left_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,DAC_LEFT_MUTE, &audio_ctrl)
```

## DAC_RIGHT_MUTE

* DAC播放右声道静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.dac.right_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,ADC_RIGHT_MUTE, &audio_ctrl)
```

## DAC_VOLUME

* DAC播放左右声道音量增益
* 默认增益0dB
* 传递参数个数：1
* 参数范围：0-175（-63.5dB ~ 24dB）
* 使用示例： 

```cpp
audio_ctrl.dac.volume = AIC32x4_DAC_VOLUME_DEFAULT;
ioctl(s_fdTlv,DAC_VOLUME, &audio_ctrl)
```

## DAC_LEFT_VOLUME

* DAC播放左声道音量增益
* 默认增益0dB
* 传递参数个数：1
* 参数范围：0-175（-63.5dB ~ 24dB）
* 使用示例： 

```cpp
audio_ctrl.dac.left_volume = AIC32x4_DAC_VOLUME_DEFAULT;
ioctl(s_fdTlv,DAC_LEFT_VOLUME, &audio_ctrl)
```

## DAC_RIGHT_VOLUME

* DAC播放右声道音量增益
* 默认增益0dB
* 传递参数个数：1
* 参数范围：0-175（-63.5dB ~ 24dB）
* 使用示例： 

```cpp
audio_ctrl.dac.right_volume = AIC32x4_DAC_VOLUME_DEFAULT;
ioctl(s_fdTlv,DAC_RIGHT_VOLUME, &audio_ctrl)
```

## HPL_INPUT 

* headphone left 输出的数据来源
* 来源有 left_DAC、IN1L、MAL、MAR，默认打开left_DAC
* 传递参数个数：4
* 参数范围：0-1（1为有，0为无）
* 使用示例： 

```cpp
audio_ctrl.hp.ldac_hpl = 1;
audio_ctrl.hp.in1l_hpl = 0;
audio_ctrl.hp.mal_hpl = 0;
audio_ctrl.hp.mar_hpl = 0;
ioctl(s_fdTlv,HPL_INPUT , &audio_ctrl)
```

## HPR_INPUT

* headphone right输出的数据来源
* 来源有 left_DAC、right_DAC、IN1R、MAR、HPL，默认打开right_DAC
* 传递参数个数：5
* 参数范围：0-1（1为有，0为无）
* 使用示例： 

```cpp
audio_ctrl.hp.ldac_hpr = 0;
audio_ctrl.hp.rdac_hpr = 1;
audio_ctrl.hp.in1r_hpr = 0;
audio_ctrl.hp.mar_hpr = 0;
audio_ctrl.hp.hpl_hpr = 0;      //当只有单声道数据，但是希望输出立体声时，可以打开此选项
ioctl(s_fdTlv,HPR_INPUT , &audio_ctrl)
```

## HP_MUTE

* headphone 左右声道输出静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.output_mute.hp_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,HP_MUTE , &audio_ctrl)
```

## HPL_MUTE

* headphone 左声道输出静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.output_mute.hpl_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,HPL_MUTE , &audio_ctrl)
```

## HPR_MUTE

* headphone 右声道输出静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.output_mute.hpr_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,HPR_MUTE , &audio_ctrl)
```

## HP_VOLUME

* headphone 左右声道输出的音量增益
* 默认0dB
* 传递参数个数：1
* 参数范围：0-35（-6dB ~ 29dB）
* 使用示例： 

```cpp
audio_ctrl.output_volume.hp_volume = AIC32x4_HP_LO_VOLUME_DEFAULT;
ioctl(s_fdTlv,HP_VOLUME , &audio_ctrl)
```

## HPL_VOLUME

* headphone 左声道输出的音量增益
* 默认0dB
* 传递参数个数：1
* 参数范围：0-35（-6dB ~ 29dB）
* 使用示例： 

```cpp
audio_ctrl.output_volume.hpl_volume = AIC32x4_HP_LO_VOLUME_DEFAULT;
ioctl(s_fdTlv,HPL_VOLUME , &audio_ctrl)
```

## HPR_VOLUME

* headphone 右声道输出的音量增益
* 默认0dB
* 传递参数个数：1
* 参数范围：0-35（-6dB ~ 29dB）
* 使用示例： 

```cpp
audio_ctrl.output_volume.hpr_volume = AIC32x4_HP_LO_VOLUME_DEFAULT;
ioctl(s_fdTlv,HPR_VOLUME , &audio_ctrl)
```

## IN1L_HPL_VOLUME

* IN1的左声道直接路由到headphone 左声道输出的音量增益
* 默认0dB
* 传递参数个数：1
* 参数范围：0-117（其中0-116为0dB ~ -72.3dB，117为静音）
* 使用示例： 

```cpp
audio_ctrl.output_volume.in1l_hpl_volume = AIC32x4_IN1_HP_VOLUME_DEFAULT  
ioctl(s_fdTlv,IN1L_HPL_VOLUME , &audio_ctrl)
```

## IN1R_HPR_VOLUME

* IN1的右声道直接路由到headphone 右声道输出的音量增益
* 默认0dB
* 传递参数个数：1
* 参数范围：0-117（其中0-116为0dB ~ -72.3dB，117为静音）
* 使用示例： 

```cpp
audio_ctrl.output_volume.in1r_hpr_volume = AIC32x4_IN1_HP_VOLUME_DEFAULT  
ioctl(s_fdTlv,IN1R_HPR_VOLUME , &audio_ctrl)
```

## LOL_INPUT

* line out 左声道输出的数据来源
* 来源有 left_DAC、right_DAC、MAL、LOR，默认打开left_DAC
* 传递参数个数：4
* 参数范围：0-1（1为有，0为无）
* 使用示例： 

```cpp
audio_ctrl.lo.rdac_lol = 0;
audio_ctrl.lo.ldac_lol = 1;
audio_ctrl.lo.mal_lol = 0;
audio_ctrl.lo.lor_lol = 0;
ioctl(s_fdTlv,LOL_INPUT , &audio_ctrl)
```

## LOR_INPUT

* line out 右声道输出的数据来源
* 来源有 right_DAC、MAR，默认打开right_DAC
* 传递参数个数：2
* 参数范围：0-1（1为有，0为无）
* 使用示例： 

```cpp
audio_ctrl.lo.rdac_lor = 1;
audio_ctrl.lo.mar_lor = 0;
ioctl(s_fdTlv,LOR_INPUT , &audio_ctrl)
```

## LO_MUTE

* line out左右声道输出静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.output_mute.lo_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,LO_MUTE , &audio_ctrl)
```

## LOL_MUTE

* line out左声道输出静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.output_mute.lol_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,LOL_MUTE , &audio_ctrl)
```

## LOR_MUTE

* line out右声道输出静音
* 默认不静音
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.output_mute.lor_mute = AIC32x4_UNMUTE;
ioctl(s_fdTlv,LOR_MUTE , &audio_ctrl)
```

## MA_VOLUME

* MA (Mixer Amplifier) 混音器的音量，不经过ADC的音频采集，可以选择路由到HP或者LO进行输出
* 默认0dB
* 传递参数个数：1
* 参数范围：0-40（其中0-39为0dB ~ -30.1dB，40为静音）
* 使用示例： 

```cpp
audio_ctrl.output_volume.ma_volume = AIC32x4_MA_VOLUME_DEFAULT  
ioctl(s_fdTlv,MA_VOLUME , &audio_ctrl)
```

## MAL_VOLUME

* MA (Mixer Amplifier) 混音器左声道的音量
* 默认0dB
* 传递参数个数：1
* 参数范围：0-40（其中0-39为0dB ~ -30.1dB，40为静音）
* 使用示例： 

```cpp
audio_ctrl.output_volume.mal_volume = AIC32x4_MA_VOLUME_DEFAULT  
ioctl(s_fdTlv,MAL_VOLUME , &audio_ctrl)
```

## MAR_VOLUME

* MA (Mixer Amplifier) 混音器右声道的音量
* 默认0dB
* 传递参数个数：1
* 参数范围：0-40（其中0-39为0dB ~ -30.1dB，40为静音）
* 使用示例： 

```cpp
audio_ctrl.output_volume.mar_volume = AIC32x4_MA_VOLUME_DEFAULT  
ioctl(s_fdTlv,MAR_VOLUME , &audio_ctrl)
```

## MICBIAS_CONFIG

* 配置偏置电压，用于数字麦克风
* 默认不使能该功能
* 传递参数个数：3
* 使用示例： 

```cpp
audio_ctrl.other.micbias_power_up = AIC32x4_DISABLE;	//不打开
audio_ctrl.other.micbias_output_voltage = 0;			//00、01、10、11，具体见3254手册page1 reg0x33
audio_ctrl.other.micbias_voltage_source = 0;			//0-AVDD， 1-LDOIN
ioctl(s_fdTlv,MICBIAS_CONFIG , &audio_ctrl)
```

## MICPGA_LEFT_POSITIVE_INPUT

* MICPGA左声道P端音频数据来源，MICPGA是音频经过ADC之后的处理模块
* 默认情况下，IN1/IN2/IN3的选项全部打开，CM不打开
* 传递参数个数：4
* 使用示例： 

```cpp
audio_ctrl.micpga_left.in1l_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_left.in2l_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_left.in3l_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_left.in1r_micpga = AIC32x4_ROUTED_WITH_10K_RES;
ioctl(s_fdTlv,MICPGA_LEFT_POSITIVE_INPUT , &audio_ctrl)
```

## MICPGA_LEFT_NEGTIVE_INPUT

* MICPGA左声道N端音频数据来源，MICPGA是音频经过ADC之后的处理模块
* 默认情况下，IN1/IN2/IN3的选项全部打开，CM不打开
* 传递参数个数：4
* 使用示例： 

```cpp
audio_ctrl.micpga_left.cm_micpga_via_cm1l = AIC32x4_NOT_ROUTED;
audio_ctrl.micpga_left.in2r_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_left.in3r_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_left.cm_micpga_via_cm2l = AIC32x4_NOT_ROUTED;
ioctl(s_fdTlv,MICPGA_LEFT_NEGTIVE_INPUT , &audio_ctrl)
```

## MICPGA_RIGHT_POSITIVE_INPUT

* MICPGA右声道P端音频数据来源，MICPGA是音频经过ADC之后的处理模块
* 默认情况下，IN1/IN2/IN3的选项全部打开，CM不打开
* 传递参数个数：4
* 使用示例： 

```cpp
audio_ctrl.micpga_right.in1r_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_right.in2r_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_right.in3r_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_right.in1l_micpga = AIC32x4_ROUTED_WITH_10K_RES;
ioctl(s_fdTlv,MICPGA_RIGHT_POSITIVE_INPUT , &audio_ctrl)
```

## MICPGA_RIGHT_NEGTIVE_INPUT

* MICPGA右声道N端音频数据来源，MICPGA是音频经过ADC之后的处理模块
* 默认情况下，IN1/IN2/IN3的选项全部打开，CM不打开
* 传递参数个数：4
* 使用示例： 

```cpp
audio_ctrl.micpga_right.cm_micpga_via_cm1r = AIC32x4_NOT_ROUTED;
audio_ctrl.micpga_right.in2l_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_right.in3l_micpga = AIC32x4_ROUTED_WITH_10K_RES;
audio_ctrl.micpga_right.cm_micpga_via_cm2r = AIC32x4_NOT_ROUTED;
ioctl(s_fdTlv,MICPGA_RIGHT_NEGTIVE_INPUT , &audio_ctrl)
```

## FLOATING_INPUT

* 浮空输入设置，当IN1/IN2/IN3不route到MICPGA或HP上，可以打开此选项，让INx路由到CM上
* 传递参数个数：6
* 使用示例： 

```cpp
audio_ctrl.float_input.in1l_cm = AIC32x4_DISABLE;
audio_ctrl.float_input.in1r_cm = AIC32x4_DISABLE;
audio_ctrl.float_input.in2l_cm = AIC32x4_DISABLE;
audio_ctrl.float_input.in2r_cm = AIC32x4_DISABLE;
audio_ctrl.float_input.in3l_cm = AIC32x4_DISABLE;
audio_ctrl.float_input.in3r_cm = AIC32x4_DISABLE;
ioctl(s_fdTlv,FLOATING_INPUT, &audio_ctrl)
```

## MICPGA_VOLUME

* MICPGA音量增益控制（左右声道的采集音量）
* 传递参数个数：2，注意一定要加上power_up参数来使能它
* 音量范围：0-95（0dB ~ 47.5dB）
* 使用示例： 

```cpp
audio_ctrl.micpga_volume.power_up = AIC32x4_ENABLE;
audio_ctrl.micpga_volume.volume = AIC32x4_MICPGA_VOLUME_DEFAULT;
ioctl(s_fdTlv,MICPGA_VOLUME, &audio_ctrl)
```

## MICPGA_LEFT_VOLUME

* MICPGA左声道音量增益控制（左声道的采集音量）
* 传递参数个数：2，注意一定要加上power_up参数来使能它
* 音量范围：0-95（0dB ~ 47.5dB）
* 使用示例： 

```cpp
audio_ctrl.micpga_volume.left_power_up = AIC32x4_ENABLE;
audio_ctrl.micpga_volume.left_volume = 10;
ioctl(s_fdTlv,MICPGA_LEFT_VOLUME, &audio_ctrl)
```

## MICPGA_RIGHT_VOLUME

* MICPGA右声道音量增益控制（右声道的采集音量）
* 传递参数个数：2，注意一定要加上power_up参数来使能它
* 音量范围：0-95（0dB ~ 47.5dB）
* 使用示例： 

```cpp
audio_ctrl.micpga_volume.right_power_up = AIC32x4_ENABLE;
audio_ctrl.micpga_volume.right_volume = 10;
ioctl(s_fdTlv,MICPGA_RIGHT_VOLUME, &audio_ctrl)
```



## LINE_OUT_POWER_UP

* line out 左右声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.lo_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,LINE_OUT_POWER_UP , &audio_ctrl)
```

## LINE_OUT_LEFT_POWER_UP

* line out 左声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.lol_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,LINE_OUT_LEFT_POWER_UP , &audio_ctrl)
```

## LINE_OUT_RIGHT_POWER_UP

* line out 右声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.lor_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,LINE_OUT_RIGHT_POWER_UP , &audio_ctrl)
```

## HEADPHONE_POWER_UP

* headphone 左右声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.hp_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,HEADPHONE_POWER_UP , &audio_ctrl)
```

## HEADPHONE_LEFT_POWER_UP

* headphone  左声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.hpl_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,HEADPHONE_LEFT_POWER_UP , &audio_ctrl)
```

## HEADPHONE_RIGHT_POWER_UP

* headphone  右声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.hpr_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,HEADPHONE_RIGHT_POWER_UP , &audio_ctrl)
```

## MA_POWER_UP

* Mixer Amplifier 混音器左右声道输出使能，混音器中的音频数据不经过ADC，直接采集后输出
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.ma_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,MA_POWER_UP , &audio_ctrl)
```

## MA_LEFT_POWER_UP 

* Mixer Amplifier 混音器左声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.mal_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,MA_LEFT_POWER_UP, &audio_ctrl)
```

## MA_RIGHT_POWER_UP

* Mixer Amplifier 混音器右声道输出使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.powerup.mar_power_up = AIC32x4_ENABLE;
ioctl(s_fdTlv,MA_RIGHT_POWER_UP, &audio_ctrl)
```

## ADC_SIGNAL_PROC_BLOCK

* ADC信号处理算法
* 默认1，参数范围1-18，具体见3254手册page0 reg0x3d
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.other.adc_signal_block = 1;
ioctl(s_fdTlv,ADC_SIGNAL_PROC_BLOCK,&audio_ctrl);
```

## DAC_SIGNAL_PROC_BLOCK

* DAC信号处理算法
* 默认1，参数范围1-25，具体见3254手册page0 reg0x3c
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.other.dac_signal_block = 1;
ioctl(s_fdTlv,DAC_SIGNAL_PROC_BLOCK,&audio_ctrl);
```



## AGC_POWER_UP 

* 左右声道AGC自动增益控制使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.agc_power_up = 1;
ioctl(s_fdTlv,AGC_POWER_UP,&audio_ctrl);
```

## AGC_LEFT_POWER_UP

* 左声道AGC自动增益控制使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.left_agc_power_up = 0;
ioctl(s_fdTlv,AGC_LEFT_POWER_UP,&audio_ctrl);
```

## AGC_RIGHT_POWER_UP

* 右声道AGC自动增益控制使能
* 默认使能
* 传递参数个数：1
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.right_agc_power_up = 0;
ioctl(s_fdTlv,AGC_RIGHT_POWER_UP,&audio_ctrl);
```
## AGC_TARGET_LEVEL 

* 左右声道AGC Target Level Setting
* 传递参数个数：1
* 参数范围：0-7（ -5.5dBFS ~  -24.0dBFS）
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.target_level = 2;
ioctl(s_fdTlv,AGC_TARGET_LEVEL,&audio_ctrl);
```

## AGC_LEFT_TARGET_LEVEL 

* 左声道AGC Target Level Setting
* 传递参数个数：1
* 参数范围：0-7（ -5.5dBFS ~  -24.0dBFS）
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.left_target_level = 2;
ioctl(s_fdTlv,AGC_LEFT_TARGET_LEVEL ,&audio_ctrl);
```

## AGC_RIGHT_TARGET_LEVEL

* 右声道AGC Target Level Setting
* 传递参数个数：1
* 参数范围：0-7（ -5.5dBFS ~  -24.0dBFS）
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.right_target_level = 2;
ioctl(s_fdTlv,AGC_RIGHT_TARGET_LEVEL,&audio_ctrl);
```

## AGC_GAIN_HYSTERESIS

* 左右声道AGC Gain Hysteresis Control
* 传递参数个数：1
* 参数范围：0-3（ 0-disable,  1 ~ 3 : ±0.5dB ~ ±1.5dB）
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.gain_hysteresis = 1;
ioctl(s_fdTlv,AGC_GAIN_HYSTERESIS,&audio_ctrl);
```

## AGC_LEFT_GAIN_HYSTERESIS 

* 左声道AGC Gain Hysteresis Control
* 传递参数个数：1
* 参数范围：0-3（ 0-disable,  1 ~ 3 : ±0.5dB ~ ±1.5dB）
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.left_gain_hysteresis = 1;
ioctl(s_fdTlv,AGC_LEFT_GAIN_HYSTERESIS  ,&audio_ctrl);
```

## AGC_RIGHT_GAIN_HYSTERESIS

* 右声道AGC Gain Hysteresis Control
* 传递参数个数：1
* 参数范围：0-3（ 0-disable,  1 ~ 3 : ±0.5dB ~ ±1.5dB）
* 使用示例： 

```cpp
audio_ctrl.agc_conf1.right_gain_hysteresis = 1;
ioctl(s_fdTlv,AGC_RIGHT_GAIN_HYSTERESIS,&audio_ctrl);
```

## AGC_HYSTERESIS

* 左右声道AGC Hysteresis Setting
* 传递参数个数：1
* 参数范围：0-3（ 3-disable,  0 ~ 2 : 1dB ~ 2dB ~ 4dB）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.hysteresis = 1;
ioctl(s_fdTlv,AGC_HYSTERESIS,&audio_ctrl);
```

## AGC_LEFT_HYSTERESIS

* 左声道AGC Hysteresis Setting
* 传递参数个数：1
* 参数范围：0-3（ 3-disable,  0 ~ 2 : 1dB ~ 2dB ~ 4dB）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.left_hysteresis = 1;
ioctl(s_fdTlv,AGC_LEFT_HYSTERESIS  ,&audio_ctrl);
```

## AGC_RIGHT_HYSTERESIS

* 右声道AGC Hysteresis Setting
* 传递参数个数：1
* 参数范围：0-3（ 3-disable,  0 ~ 2 : 1dB ~ 2dB ~ 4dB）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.right_hysteresis = 1;
ioctl(s_fdTlv,AGC_RIGHT_HYSTERESIS,&audio_ctrl);
```

## AGC_NOISE_THRESHOLD

* 左右声道AGC Noise Threshold
* 传递参数个数：1
* 参数范围：0-31（ 0-disable,  1 ~ 31 : -30dB ~ 90dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.noise_threshold = 4;
ioctl(s_fdTlv,AGC_NOISE_THRESHOLD,&audio_ctrl);
```

## AGC_LEFT_NOISE_THRESHOLD

* 左声道AGC Noise Threshold
* 传递参数个数：1
* 参数范围：0-31（ 0-disable,  1 ~ 31 : -30dB ~ 90dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.left_noise_threshold = 4;
ioctl(s_fdTlv,AGC_NOISE_THRESHOLD,&audio_ctrl);
```

## AGC_RIGHT_NOISE_THRESHOLD

* 右声道AGC Noise Threshold
* 传递参数个数：1
* 参数范围：0-31（ 0-disable,  1 ~ 31 : -30dB ~ 90dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.right_noise_threshold = 4;
ioctl(s_fdTlv,AGC_NOISE_THRESHOLD,&audio_ctrl);
```

## AGC_MAX_GAIN

* 左右声道AGC Maximum Gain Setting
* 传递参数个数：1
* 参数范围：0-116（ 0dB ~ 58dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.max_gain = 4;
ioctl(s_fdTlv,AGC_MAX_GAIN,&audio_ctrl);
```

## AGC_LEFT_MAX_GAIN

* 左声道AGC Maximum Gain Setting
* 传递参数个数：1
* 参数范围：0-116（ 0dB ~ 58dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.left_max_gain = 4;
ioctl(s_fdTlv,AGC_LEFT_MAX_GAIN,&audio_ctrl);
```

## AGC_RIGHT_MAX_GAIN

* 右声道AGC Maximum Gain Setting
* 传递参数个数：1
* 参数范围：0-116（ 0dB ~ 58dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf2.right_max_gain = 4;
ioctl(s_fdTlv,AGC_RIGHT_MAX_GAIN,&audio_ctrl);
```

## AGC_ATTACK_TIME

* 左右声道AGC Attack Time Setting
* 传递参数个数：1
* 参数范围：0-31（ 1 * 32 ADC Word Clocks  ~  63 * 32 ADC Word Clocks ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.attack_time = 4;
ioctl(s_fdTlv,AGC_ATTACK_TIME,&audio_ctrl);
```

## AGC_LEFT_ATTACK_TIME

* 左声道AGC Attack Time Setting
* 传递参数个数：1
* 参数范围：0-31（ 1 * 32 ADC Word Clocks  ~  63 * 32 ADC Word Clocks ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.left_attack_time = 4;
ioctl(s_fdTlv,AGC_LEFT_ATTACK_TIME,&audio_ctrl);
```

## AGC_RIGHT_ATTACK_TIME

* 右声道AGC Attack Time Setting
* 传递参数个数：1
* 参数范围：0-31（ 1 * 32 ADC Word Clocks  ~  63 * 32 ADC Word Clocks ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.right_attack_time = 4;
ioctl(s_fdTlv,AGC_RIGHT_ATTACK_TIME,&audio_ctrl);
```



## AGC_ATTACK_TIME_SCALE_FACTOR

* 左右声道AGC Attack Time Scale Factor Setting
* 传递参数个数：1
* 参数范围：0-127（ 1  ~  128 ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.attack_time_scale_factor = 1;
ioctl(s_fdTlv,AGC_ATTACK_TIME_SCALE_FACTOR,&audio_ctrl);
```

## AGC_LEFT_ATTACK_TIME_SCALE_FACTOR

* 左声道AGC Attack Time Scale Factor Setting
* 传递参数个数：1
* 参数范围：0-127（ 1  ~  128 ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.left_attack_time_scale_factor = 1;
ioctl(s_fdTlv,AGC_LEFT_ATTACK_TIME_SCALE_FACTOR,&audio_ctrl);
```

## AGC_RIGHT_ATTACK_TIME_SCALE_FACTOR

* 右声道AGC Attack Time Scale Factor Setting
* 传递参数个数：1
* 参数范围：0-127（ 1  ~  128 ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.right_attack_time_scale_factor = 1;
ioctl(s_fdTlv,AGC_RIGHT_ATTACK_TIME_SCALE_FACTOR,&audio_ctrl);
```

## AGC_DECAY_TIME

* 左右声道AGC Decay Time Setting
* 传递参数个数：1
* 参数范围：0-31（ 1 * 32 ADC Word Clocks  ~  63 * 32 ADC Word Clocks ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.decay_time = 4;
ioctl(s_fdTlv,AGC_DECAY_TIME,&audio_ctrl);
```

## AGC_LEFT_DECAY_TIME

* 左声道AGC Decay Time Setting
* 传递参数个数：1
* 参数范围：0-31（ 1 * 32 ADC Word Clocks  ~  63 * 32 ADC Word Clocks ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.left_decay_time = 4;
ioctl(s_fdTlv,AGC_LEFT_DECAY_TIME,&audio_ctrl);
```

## AGC_RIGHT_DECAY_TIME

* 右声道AGC Decay Time Setting
* 传递参数个数：1
* 参数范围：0-31（ 1 * 32 ADC Word Clocks  ~  63 * 32 ADC Word Clocks ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.right_decay_time = 4;
ioctl(s_fdTlv,AGC_RIGHT_DECAY_TIME,&audio_ctrl);
```

## AGC_DECAY_TIME_SCALE_FACTOR

* 左右声道AGC Decay Time Scale Factor Setting
* 传递参数个数：1
* 参数范围：0-127（ 1  ~  128 ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.decay_time_scale_factor = 1;
ioctl(s_fdTlv,AGC_DECAY_TIME_SCALE_FACTOR,&audio_ctrl);
```

## AGC_LEFT_DECAY_TIME_SCALE_FACTOR

* 左声道AGC Decay Time Scale Factor Setting
* 传递参数个数：1
* 参数范围：0-127（ 1  ~  128 ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.left_decay_time_scale_factor = 1;
ioctl(s_fdTlv,AGC_LEFT_DECAY_TIME_SCALE_FACTOR,&audio_ctrl);
```

## AGC_RIGHT_DECAY_TIME_SCALE_FACTOR

* 右声道AGC Decay Time Scale Factor Setting
* 传递参数个数：1
* 参数范围：0-127（ 1  ~  128 ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf3.right_decay_time_scale_factor = 1;
ioctl(s_fdTlv,AGC_RIGHT_DECAY_TIME_SCALE_FACTOR,&audio_ctrl);
```

## AGC_NOISE_DEBOUNCE_TIME

* 左右声道AGC Noise Debounce Time Setting
* 传递参数个数：1
* 参数范围：0-31（具体见3254手册page0 reg0x5b ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.noise_debounce_time = 1;
ioctl(s_fdTlv,AGC_NOISE_DEBOUNCE_TIME,&audio_ctrl);
```

## AGC_LEFT_NOISE_DEBOUNCE_TIME

* 左声道AGC Noise Debounce Time Setting
* 传递参数个数：1
* 参数范围：0-31（具体见3254手册page0 reg0x5b ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.left_noise_debounce_time = 1;
ioctl(s_fdTlv,AGC_LEFT_NOISE_DEBOUNCE_TIME,&audio_ctrl);
```

## AGC_RIGHT_NOISE_DEBOUNCE_TIME

* 右声道AGC Noise Debounce Time Setting
* 传递参数个数：1
* 参数范围：0-31（具体见3254手册page0 reg0x5b ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.right_noise_debounce_time = 1;
ioctl(s_fdTlv,AGC_RIGHT_NOISE_DEBOUNCE_TIME,&audio_ctrl);
```

## AGC_SIGNAL_DEBOUNCE_TIME

* 左右声道AGC Signal Debounce Time Setting
* 传递参数个数：1
* 参数范围：0-15（具体见3254手册page0 reg0x5c ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.signal_debounce_time = 1;
ioctl(s_fdTlv,AGC_SIGNAL_DEBOUNCE_TIME,&audio_ctrl);
```

## AGC_LEFT_SIGNAL_DEBOUNCE_TIME

* 左声道AGC Signal Debounce Time Setting
* 传递参数个数：1
* 参数范围：0-15（具体见3254手册page0 reg0x5c ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.left_signal_debounce_time = 2;
ioctl(s_fdTlv,AGC_LEFT_SIGNAL_DEBOUNCE_TIME,&audio_ctrl);
```

## AGC_RIGHT_SIGNAL_DEBOUNCE_TIME

* 右声道AGC Signal Debounce Time Setting
* 传递参数个数：1
* 参数范围：0-15（具体见3254手册page0 reg0x5c ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.right_signal_debounce_time = 4;
ioctl(s_fdTlv,AGC_RIGHT_SIGNAL_DEBOUNCE_TIME,&audio_ctrl);
```

## AGC_GAIN

* 左右声道AGC Gain Control
* 传递参数个数：1
* 参数范围：0-140（-12dB ~ +58dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.agc_gain = 1;
ioctl(s_fdTlv,AGC_GAIN,&audio_ctrl);
```

## AGC_LEFT_GAIN

* 左声道AGC Gain Control
* 传递参数个数：1
* 参数范围：0-140（-12dB ~ +58dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.left_agc_gain = 2;
ioctl(s_fdTlv,AGC_LEFT_GAIN,&audio_ctrl);
```

## AGC_RIGHT_GAIN

* 右声道AGC Gain Control
* 传递参数个数：1
* 参数范围：0-140（-12dB ~ +58dB ）
* 使用示例： 

```cpp
audio_ctrl.agc_conf4.right_agc_gain = 4;
ioctl(s_fdTlv,AGC_RIGHT_GAIN,&audio_ctrl);
```



# 初始化步骤

第一步，先打开设备，获取文件流指针fd

```cpp
#define TLV320_FILE "/dev/tlv320aic31"
int s_fdTlv = open(TLV320_FILE, O_RDWR);
```



第二步，复位并设置默认参数

```cpp
ioctl(s_fdTlv,SOFT_RESET,&audio_ctrl)
```



第三步，设置采样率，采样精度，主从模式，传输模式等关键参数

```cpp
audio_ctrl.interface.master_slave_mode = AIC32x4_SLAVE_MODE;
ioctl(s_fdTlv,MASTER_SLAVE_MODE, &audio_ctrl)
    
audio_ctrl.interface.transfer_mode = AIC32x4_TRANSFER_MODE_I2S;
ioctl(s_fdTlv,TRANSFER_MODE, &audio_ctrl)

audio_ctrl.interface.sample_rate = AIC32x4_SAMPLE_RATE_48K;
ioctl(s_fdTlv,SAMPLE_RATE, &audio_ctrl)
    
audio_ctrl.interface.bit_width = AIC32x4_BIT_WIDTH_16B;
audio_ctrl.interface.chn_num = pstAioAttr->u32ChnCnt;
ioctl(s_fdTlv,BIT_WIDTH, &audio_ctrl);
```

第四步，设置其它参数，如音量