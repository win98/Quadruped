#ifndef _SERVO_CONTROLLER_H_
#define _SERVO_CONTROLLER_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_gpio.h"


/** @defgroup concrete hardware-dependent definitions
  * @{
  */
#define CORE_FCLK               40000000    //Core Frequency
#define TIM_FCLK                1000000     //Timer frequency, should be more than 1,000,000
#define DEGREES_0_PULSE_US      1500
#define DEGREES_90_PULSE_US     2000


/** 
  * @brief  Channel polarity normal and inverse enumeration 
  */
typedef enum
{
    SERVO_CHANNEL_NORMAL,                             /*!< Normal polarity of servo control pulse       */
    SERVO_CHANNEL_INVERSE                             /*!< Inverse polarity of servo control pulse      */
} 
SERVO_ChannelPolarity;


/** 
  * @brief  ServoController channels enumeration 
  */
typedef enum
{
    SERVO_Channel_1 = 0,                /*!< All available channels are enumerated here  */
    SERVO_Channel_2,
    SERVO_Channel_3,
    SERVO_Channel_4,
    SERVO_Channel_5,
    SERVO_Channel_6,
    SERVO_Channel_7,
    SERVO_Channel_8,
    SERVO_Channel_9,
    SERVO_Channel_10,
    SERVO_Channel_11,
    SERVO_Channel_12,
    SERVO_Channel_13,
    SERVO_Channel_14,
    SERVO_Channel_15,
    SERVO_Channel_16,
    SERVO_Channel_17,
    SERVO_Channel_18,
    SERVO_Channel_19,
    SERVO_Channel_20,
    SERVO_Channel_21,
    SERVO_Channel_22,
    SERVO_Channel_23,
    SERVO_Channel_24,
    
    SERVO_TotalChannelsNum
} 
SERVO_ChannelId;


/** 
  * @brief   ServoController channel structure definition  
  */
typedef struct
{
    uint16_t pulseWidth_us;
    uint16_t pulseCycles;
} 
SERVO_Channel;


/** 
  * @brief  ServoController operating frequency enumeration 
  */
typedef enum
{
    SERVO_FREQ_50HZ  = 50,          /*!< Generating rising edge of control pulses at 50 Hz  */
    SERVO_FREQ_100HZ = 100,         /*!< Generating rising edge of control pulses at 100 Hz  */
    SERVO_FREQ_200HZ = 200,         /*!< Generating rising edge of control pulses at 200 Hz  */
    SERVO_FREQ_300HZ = 300          /*!< Generating rising edge of control pulses at 300 Hz  */
} 
SERVO_CONTROLLER_Frequency;


/* ServoController public functions  ************************************************/
void SERVO_CONTROLLER_Init (SERVO_CONTROLLER_Frequency frequency);
void SERVO_CONTROLLER_Start (void);
void SERVO_CONTROLLER_SetChannel (SERVO_ChannelId channel, uint16_t microseconds);
void SERVO_CONTROLLER_SetChannels (uint16_t *microseconds);

#endif
