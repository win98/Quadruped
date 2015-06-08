/**
  ******************************************************************************
  * File Name          : main.c
  * Date               : 28/05/2015 15:41:59
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "WiFiCommModule.h"
#include "ServoProtocolMaster.h"

#include "QuadrupedLeg.h"
#include "QuadrupedCreepGait.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

osThreadId defaultTaskHandle;
osThreadId wfCommTaskHandle;
osThreadId quadrTaskHandle;

xQueueHandle wfQueue;

ROBOT_CONTROL_MODE controlMode;
int8_t straightVelocity;
int8_t lateralVelocity;
int8_t angularVelocity;

uint16_t legsPulses[24];

/* USER CODE BEGIN PV */

#define SERVO_CHANNELS_NUMBER       24

#define WIFI_RESET_PORT     GPIOB
#define WIFI_RESET_PIN      GPIO_PIN_15

#define DEFAULT_SERVO_POSITION_PULSE    1500

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);
void quadrupedTask(void const * argument);

/* USER CODE BEGIN PFP */
#define WF_QUEUE_SIZE   1
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  
    // Init ServoController communication protocol.
    SERVO_PROTOCOL_Init();
    /*
    // Set default servos positions.
    uint16_t channelsValues[SERVO_CHANNELS_NUMBER];
    for (uint8_t i = 0; i < SERVO_CHANNELS_NUMBER; i++)
    {
        channelsValues[i] = DEFAULT_SERVO_POSITION_PULSE;
    }
    SERVO_PROTOCOL_SendCommand (SERVO_SET_CHANNELS, 0, channelsValues, SERVO_CHANNELS_NUMBER);
    */
  
    // Init WiFi.
    HAL_GPIO_WritePin(WIFI_RESET_PORT, WIFI_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(WIFI_RESET_PORT, WIFI_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay(50);
    WF_Init();
   
    
    // Init variables.
    straightVelocity = 0;
    lateralVelocity = 0;
    angularVelocity = 0;
    
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  
  
  
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
  // WiFi communication thread.
  osThreadDef(wfCommTask, WF_CommThread, osPriorityNormal, 0, 128);
  wfCommTaskHandle = osThreadCreate(osThread(wfCommTask), NULL);
  
  osThreadDef(quadrTask, quadrupedTask, osPriorityNormal, 0, 128);
  quadrTaskHandle = osThreadCreate(osThread(quadrTask), NULL);
  
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  
  // Incoming robot commands queue.
  wfQueue = xQueueCreate(WF_QUEUE_SIZE, sizeof(WF_Robo_Packet *));
  WF_SetRxQueue(wfQueue);
  
  /* USER CODE END RTOS_QUEUES */
 
  
  /* Start scheduler */
  osKernelStart(NULL, NULL);
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 20;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
        * Free pins are configured automatically as Analog (this feature is enabled through 
        * the Code Generation settings)
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOE_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PE2 PE3 PE4 PE5 
                           PE6 PE7 PE8 PE9 
                           PE10 PE11 PE12 PE13 
                           PE14 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC14 PC15 PC0 
                           PC1 PC2 PC3 PC4 
                           PC5 PC6 PC7 PC8 
                           PC9 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0 
                          |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 PA5 
                           PA6 PA7 PA8 PA9 
                           PA10 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10 
                           PB11 PB12 PB13 PB14 
                           PB5 PB6 PB7 PB8 
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10 
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD10 PD13 PD14 PD15 
                           PD0 PD1 PD2 PD3 
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
    WF_Robo_Packet *packet;
    uint8_t value;
    int8_t value_s;
    uint16_t *data16;
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
    for(;;)
    {
        xQueueReceive(wfQueue, &packet, portMAX_DELAY);
        
        // Process packet here
        switch (packet->cmd)
        {
            case WF_ROBOT_SET_CHANNEL:
                if (controlMode == RCM_DIRECT_SERVO_MODE)
                {
                    uint8_t channel = packet->data[0];              // Number of channel.
                    data16 = (uint16_t *)(&packet->data[1]);        // Value of channel.
                    SERVO_PROTOCOL_SendCommand (SERVO_SET_CHANNEL, channel, data16, 1);
                }
                
                break;
                
            case WF_ROBOT_SET_CHANNELS:
                if (controlMode == RCM_DIRECT_SERVO_MODE)
                {
                    uint8_t channelsNum = packet->data[0];
                    
                    if (channelsNum == SERVO_CHANNELS_NUMBER)
                    {
                        data16 = (uint16_t *)(&packet->data[1]);
                        SERVO_PROTOCOL_SendCommand (SERVO_SET_CHANNELS, 0, data16, channelsNum);
                    }
                }
                
                break;
                
            case WF_ROBOT_SET_CONTROL_MODE:
                value = packet->data[0];    //Mode
                controlMode = (ROBOT_CONTROL_MODE)value;
                
                break;
                
            case WF_ROBOT_SET_STRAIGHT_VELOCITY:
                value_s = (int8_t)packet->data[0];
                if (value_s >= -8 && value_s <= 8)
                {
                    straightVelocity = value_s;
                }
                
                break;
                
            case WF_ROBOT_SET_LATERAL_VELOCITY:
                value_s = (int8_t)packet->data[0];
                if (value_s >= -8 && value_s <= 8)
                {
                    lateralVelocity = value_s;
                }
                
                break;
            
            case WF_ROBOT_SET_ANGULAR_VELOCITY:
                value_s = (int8_t)packet->data[0];
                if (value_s >= -8 && value_s <= 8)
                {
                    angularVelocity = value_s;
                }
                
                break;
                
            default:
                break;
        }
        
        
        WF_RobotPacketProcessed (packet);
        
        osDelay(1);
    }
  /* USER CODE END 5 */ 
}

// Legs physical dimensions.
#define COXA_LEN    5.5f
#define FEMUR_LEN   6.0f
#define TIBIA_LEN   8.5f

// Idle position of foot.
#define IDLE_OFFSET_X     1.0f
#define IDLE_OFFSET_Y     10.0f
#define IDLE_OFFSET_Z     1.0f

// Indices of ServoController channels dedicated to angles.
#define RF_COXA_ANGLE_IDX           7
#define RF_COXA_FEMUR_ANGLE_IDX     6
#define RF_FEMUR_TIBIA_ANGLE_IDX    3

#define RH_COXA_ANGLE_IDX           2
#define RH_COXA_FEMUR_ANGLE_IDX     1
#define RH_FEMUR_TIBIA_ANGLE_IDX    0

#define LF_COXA_ANGLE_IDX           13
#define LF_COXA_FEMUR_ANGLE_IDX     14
#define LF_FEMUR_TIBIA_ANGLE_IDX    15

#define LH_COXA_ANGLE_IDX           8
#define LH_COXA_FEMUR_ANGLE_IDX     12
#define LH_FEMUR_TIBIA_ANGLE_IDX    23

#define RF_ANGLE_OFFSET         M_PI_4
#define RH_ANGLE_OFFSET        -M_PI_4
#define LF_ANGLE_OFFSET         M_PI_4 * 3.0f
#define LH_ANGLE_OFFSET        -M_PI_4 * 3.0f

#define PULSE_WIDTH_PER_PI_2        700.0f

#define COXA_FEMUR_ANGLE_OFFSET     0.15f   // Offset in hardware.
#define FEMUR_TIBIA_ANGLE_OFFSET   1.57f   // Offset in hardware.

void quadrupedTask(void const * argument)
{
    QuadrLeg *RF, *RH, *LF, *LH;
    QuadrCreepGait *creepGait;
    
    // Create legs with idle position.
    RF = quadrLegCreate(COXA_LEN, FEMUR_LEN, TIBIA_LEN, MakeQVec(  IDLE_OFFSET_X,    -IDLE_OFFSET_Y,     IDLE_OFFSET_Z));
    RH = quadrLegCreate(COXA_LEN, FEMUR_LEN, TIBIA_LEN, MakeQVec(  IDLE_OFFSET_X,    -IDLE_OFFSET_Y,    -IDLE_OFFSET_Z));
    LF = quadrLegCreate(COXA_LEN, FEMUR_LEN, TIBIA_LEN, MakeQVec( -IDLE_OFFSET_X,    -IDLE_OFFSET_Y,     IDLE_OFFSET_Z));
    LH = quadrLegCreate(COXA_LEN, FEMUR_LEN, TIBIA_LEN, MakeQVec( -IDLE_OFFSET_X,    -IDLE_OFFSET_Y,    -IDLE_OFFSET_Z));

    creepGait = quadrupedCreepGaitCreate(RF, RH, LF, LH);

    quadrupedCreepGaitStart(creepGait);
    quadrupedCreepGaitSetStraightVelocity(creepGait, 0);
    
    for (uint8_t i = 0; i < SERVO_CHANNELS_NUMBER; i++)
    {
        legsPulses[i] = 0;
    }
    
    while (1)
    {
        float step = 0.001f;
        
        quadrupedCreepGaitUpdate(creepGait, step);
        
        float f = PULSE_WIDTH_PER_PI_2 / M_PI_2;
        
        // RF leg.
        legsPulses[RF_COXA_ANGLE_IDX] = (uint16_t)(1500.0f + (RF->coxaAngle - RF_ANGLE_OFFSET) * f);
        legsPulses[RF_COXA_FEMUR_ANGLE_IDX] = (uint16_t)(1500.0f - (RF->coxaFemurAngle + COXA_FEMUR_ANGLE_OFFSET - M_PI_2) * f);
        legsPulses[RF_FEMUR_TIBIA_ANGLE_IDX] = (uint16_t)(1500.0f - (RF->femurTibiaAngle - FEMUR_TIBIA_ANGLE_OFFSET) * f);
        
        // RH leg.
        legsPulses[RH_COXA_ANGLE_IDX] = (uint16_t)(1500.0f + (RH->coxaAngle - RH_ANGLE_OFFSET) * f);
        legsPulses[RH_COXA_FEMUR_ANGLE_IDX] = (uint16_t)(1500.0f - (RH->coxaFemurAngle + COXA_FEMUR_ANGLE_OFFSET - M_PI_2) * f);
        legsPulses[RH_FEMUR_TIBIA_ANGLE_IDX] = (uint16_t)(1500.0f - (RH->femurTibiaAngle - FEMUR_TIBIA_ANGLE_OFFSET) * f);
        
        // LF leg.
        float angle = LF->coxaAngle;
        float offset = LF_ANGLE_OFFSET;
        offset = angle > 0 ? offset : offset - M_PI * 2.0f;
        legsPulses[LF_COXA_ANGLE_IDX] = (uint16_t)(1500.0f + (angle - offset) * f);
        legsPulses[LF_COXA_FEMUR_ANGLE_IDX] = (uint16_t)(1500.0f - (LF->coxaFemurAngle + COXA_FEMUR_ANGLE_OFFSET - M_PI_2) * f);
        legsPulses[LF_FEMUR_TIBIA_ANGLE_IDX] = (uint16_t)(1500.0f - (LF->femurTibiaAngle - FEMUR_TIBIA_ANGLE_OFFSET) * f);
        
        // LH leg.
        angle = LH->coxaAngle;
        offset = LH_ANGLE_OFFSET;
        offset = angle < M_PI_4 ? offset : offset + M_PI * 2.0f;        
        legsPulses[LH_COXA_ANGLE_IDX] = (uint16_t)(1500.0f + (angle - offset) * f);
        legsPulses[LH_COXA_FEMUR_ANGLE_IDX] = (uint16_t)(1500.0f - (LH->coxaFemurAngle + COXA_FEMUR_ANGLE_OFFSET - M_PI_2) * f);
        legsPulses[LH_FEMUR_TIBIA_ANGLE_IDX] = (uint16_t)(1500.0f - (LH->femurTibiaAngle - FEMUR_TIBIA_ANGLE_OFFSET) * f);
        
        SERVO_PROTOCOL_SendCommand (SERVO_SET_CHANNELS, 0, legsPulses, SERVO_CHANNELS_NUMBER);
    }
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
