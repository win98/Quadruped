/**
  ******************************************************************************
  * File Name          : main.c
  * Date               : 23/05/2015 10:17:59
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
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "ServoController.h"
#include "ServoProtocol.h"
#include <stdbool.h>
#include <stdint.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;
const uint32_t kServoCommSpeed = 1000000;
/* External variables --------------------------------------------------------*/
extern void (*UART_RcvCallback) (uint8_t data);

/* USER CODE BEGIN PV */
#define MIN_SET_CHANNEL_CMD_DATA_LENGTH     4
#define MIN_SET_CHANNELS_CMD_DATA_LENGTH    49
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
void ServoProtocolEventCallback (SERVO_PROTOCOL_Event *event);
/* Extern function prototypes -----------------------------------------------*/
extern void SERVO_PROTOCOL_RcvCallback(uint8_t data);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
//  UART_RcvCallback = SERVO_PROTOCOL_RcvCallback;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();

  /* USER CODE BEGIN 2 */

  SERVO_CONTROLLER_Init(SERVO_FREQ_200HZ);
  SERVO_CONTROLLER_Start();
  
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_1, 1000);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_2, 1100);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_3, 1200);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_4, 1300);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_7, 1400);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_8, 1500);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_9, 1600);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_13, 1700);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_14, 1800);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_15, 1900);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_16, 2000);
  SERVO_CONTROLLER_SetChannel (SERVO_Channel_24, 2100);
  
  SERVO_PROTOCOL_Init(&UART_RcvCallback, ServoProtocolEventCallback);
  
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
    
  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */

}

void ServoProtocolEventCallback (SERVO_PROTOCOL_Event *event)
{
    uint8_t *rxData = event->data;
    uint32_t rxLength = event->dataLength;
    
    SERVO_PROTOCOL_Command cmd = (SERVO_PROTOCOL_Command)rxData[0];
    uint8_t channelIdx;
    bool error = false;
    
    switch (cmd)
    {
        case SERVO_SET_CHANNEL:
            if (rxLength >= MIN_SET_CHANNEL_CMD_DATA_LENGTH)
            {
                channelIdx = rxData[1];
                if (channelIdx < SERVO_TotalChannelsNum)
                {
                    // Value comes in Little-endian format.
                    // ARM on board also works in Little-endian format.
                    // So just cast pointer to 1st byte of value to uint16_t*.
                    uint16_t *value = (uint16_t *)(rxData + 2);
                    SERVO_CONTROLLER_SetChannel((SERVO_ChannelId)channelIdx, *value);
                }
                else
                {
                    error = true;
                }
            }
            else
            {
                error = true;
            }
            break;
            
        case SERVO_SET_CHANNELS:
            if (rxLength >= MIN_SET_CHANNELS_CMD_DATA_LENGTH)
            {
                // Values come in Little-endian format.
                // ARM on board also works in Little-endian format.
                // So just cast poionter to 1st byte of value to uint16_t*.
                uint16_t *values = (uint16_t *)(rxData + 1);
                SERVO_CONTROLLER_SetChannels(values);
            }
            else
            {
                error = true;
            }
            break;
            
        default:
            break;
    }
    
    if (error)
    {
        //do nothing
    }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

}

/* USART3 init function */
void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = kServoCommSpeed;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA8 PA9 
                           PA10 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_10|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB12 PB13 
                           PB14 PB15 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
