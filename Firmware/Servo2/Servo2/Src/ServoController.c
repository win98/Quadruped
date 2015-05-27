#include "ServoController.h"
#include <stdlib.h>
#include <stdbool.h>

#define US_TO_CYCLES(us) (TIM_FCLK / 1000000 * us);  //MACRO for conversion from us to timer ticks

//Timers handlers
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

//Servo channels structs storage
SERVO_Channel servo_channels[SERVO_TotalChannelsNum];


//HARDWARE CONFIG TABLES
//Timers channels
int32_t tim_channels_table[SERVO_TotalChannelsNum] = 
{
    TIM_CHANNEL_1,  TIM_CHANNEL_2,  TIM_CHANNEL_3,  TIM_CHANNEL_4,      //1 to 4
               -1,             -1,  TIM_CHANNEL_1,  TIM_CHANNEL_2,      //5 to 8
    TIM_CHANNEL_3,             -1,             -1,             -1,      //9 to 12
    TIM_CHANNEL_4,  TIM_CHANNEL_3,  TIM_CHANNEL_2,  TIM_CHANNEL_1,      //13 to 16
               -1,             -1,             -1,             -1,      //17 to 20
               -1,             -1,             -1,  TIM_CHANNEL_4       //21 to 24
};

//Timers handlers
TIM_HandleTypeDef *tim_handlers_table[SERVO_TotalChannelsNum] = 
{
           &htim2,          &htim2,         &htim2,        &htim2,      //1 to 4
             NULL,            NULL,         &htim3,        &htim3,      //5 to 8
           &htim3,            NULL,           NULL,          NULL,      //9 to 12
           &htim4,          &htim4,         &htim4,        &htim4,      //13 to 16
             NULL,            NULL,           NULL,          NULL,      //17 to 20
             NULL,            NULL,           NULL,        &htim1       //21 to 24
};

//Private functions
/* TIM1 init function */
void SC_TIM1_Init(SERVO_CONTROLLER_Frequency frequency);
/* TIM2 init function */
void SC_TIM2_Init(SERVO_CONTROLLER_Frequency frequency);
/* TIM3 init function */
void SC_TIM3_Init(SERVO_CONTROLLER_Frequency frequency);
/* TIM4 init function */
void SC_TIM4_Init(SERVO_CONTROLLER_Frequency frequency);


void SERVO_CONTROLLER_Init (SERVO_CONTROLLER_Frequency frequency)
{
    //Timers init
    SC_TIM1_Init(frequency);
    SC_TIM2_Init(frequency);
    SC_TIM3_Init(frequency);
    SC_TIM4_Init(frequency);
    
    //Channels init
    for (int i = 0; i < SERVO_TotalChannelsNum; i++)
    {
        SERVO_Channel *ch = &servo_channels[i];
        ch->pulseWidth_us = DEGREES_0_PULSE_US;
        ch->pulseCycles = US_TO_CYCLES(DEGREES_0_PULSE_US);
    }
}

void SERVO_CONTROLLER_Start (void)
{
    //Start timers
    HAL_TIM_Base_Start_IT(&htim1);
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim4);
    
    //Start PWMs
    for (int i = 0; i < SERVO_TotalChannelsNum; i++)
    {
        TIM_HandleTypeDef *htim = tim_handlers_table[i];
        int32_t channel = tim_channels_table[i];
        
        if (!htim || channel < 0)
        {
            continue;
        }
        
        HAL_TIM_PWM_Start(htim, (uint32_t)channel);
        
        SERVO_Channel *ch = &servo_channels[i];
        SERVO_CONTROLLER_SetChannel((SERVO_ChannelId)i, ch->pulseCycles);
    }
}

/* TIM1 init function */
void SC_TIM1_Init(SERVO_CONTROLLER_Frequency frequency)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = CORE_FCLK / TIM_FCLK - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = (uint16_t)(TIM_FCLK / frequency);   //should not exceed 0xFFFF
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim1);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4);

}

/* TIM2 init function */
void SC_TIM2_Init(SERVO_CONTROLLER_Frequency frequency)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = CORE_FCLK / TIM_FCLK - 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = (uint16_t)(TIM_FCLK / frequency);   //should not exceed 0xFFFF
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim2);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);

  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);

  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);

  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4);

}

/* TIM3 init function */
void SC_TIM3_Init(SERVO_CONTROLLER_Frequency frequency)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = CORE_FCLK / TIM_FCLK - 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = (uint16_t)(TIM_FCLK / frequency);   //should not exceed 0xFFFF
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim3);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);

  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3);

}

/* TIM4 init function */
void SC_TIM4_Init(SERVO_CONTROLLER_Frequency frequency)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = CORE_FCLK / TIM_FCLK - 1;;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = (uint16_t)(TIM_FCLK / frequency);   //should not exceed 0xFFFF
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim4);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim4);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1);

  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2);

  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3);

  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4);

}

void SERVO_CONTROLLER_SetChannel (SERVO_ChannelId channel, uint16_t microseconds)
{
    SERVO_Channel *ch = &servo_channels[channel];
    TIM_HandleTypeDef *htim = tim_handlers_table[channel];
    int32_t timChannel = tim_channels_table[channel];
    
    if (!htim || timChannel < 0)
    {
        return;
    }
    
    uint16_t cycles = US_TO_CYCLES(microseconds);
    
    ch->pulseWidth_us = microseconds;
    ch->pulseCycles = cycles;
    
    /*
     * HAL driver set the Preload enable bit for all channels.
     * So we can write value directly to CCRx register.
     */
    switch (timChannel)
    {
        case TIM_CHANNEL_1:
            htim->Instance->CCR1 = cycles;
            break;
        case TIM_CHANNEL_2:
            htim->Instance->CCR2 = cycles;
            break;
        case TIM_CHANNEL_3:
            htim->Instance->CCR3 = cycles;
            break;
        case TIM_CHANNEL_4:
            htim->Instance->CCR4 = cycles;
            break;
        default:
            break;
    }
}

void SERVO_CONTROLLER_SetChannels (uint16_t *microseconds)
{
    for (int i = 0; i < SERVO_TotalChannelsNum; i++)
    {
        SERVO_CONTROLLER_SetChannel((SERVO_ChannelId)i, microseconds[i]);
    }
}
