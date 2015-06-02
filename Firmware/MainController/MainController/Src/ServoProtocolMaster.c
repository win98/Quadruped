#include "ServoProtocolMaster.h"
#include "stm32f2xx_hal.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
uint8_t servoTxBuffer[512];
const uint32_t kCommSpeed = 1000000;

/* Functions prototypes ------------------------------------------------------*/
static void SP_USART2_UART_Init(void);


void SERVO_PROTOCOL_Init (void)
{
     SP_USART2_UART_Init();
}
                          
void SERVO_PROTOCOL_SendCommand (SERVO_PROTOCOL_Command cmd, uint8_t channelNum, uint16_t *data, uint32_t dataLength)
{
    //data contains values in Little-endian format.
    //ServoController protocol uses this format too.
    
    servoTxBuffer[0] = cmd;
    
    uint32_t origLength;;
    
    switch (cmd)
    {
        case SERVO_SET_CHANNEL:
            servoTxBuffer[1] = channelNum;
            servoTxBuffer[2] = (uint8_t)data[0];            // LO byte of uint16_t
            servoTxBuffer[3] = (uint8_t)(data[0] >> 8);     // HI byte of uint16_t
            
            origLength = 4;
        
            break;
            
        case SERVO_SET_CHANNELS:
            for (int i = 0; i < dataLength; i++)
            {
                uint16_t value = data[i];
                servoTxBuffer[1 + i * 2] = (uint8_t)value;            // LO byte of uint16_t
                servoTxBuffer[2 + i * 2] = (uint8_t)(value >> 8);     // HI byte of uint16_t
            }
        
            origLength = 1 + dataLength * 2;
            
            break;   
            
        default:
            return;
            
            break;
    }
    
    // Check buffer for special symbols.
    // Every special symbol increases 
    // transmit length by 1.
    uint32_t fullLength = origLength;
    
    for (int i = 0; i < origLength; i++)
    {
        uint8_t byte = servoTxBuffer[i];
        
        if (byte == END_PACKET_SYMBOL || byte == ESC_SYMBOL)
        {
            fullLength++;
        }
    }
    
    // Replace special symbols.
    // servoTxBuffer transmit length
    // is equal to fullLength.
    if (fullLength > origLength)
    {
        int32_t origIdx = origLength - 1;
        int32_t fullIdx = fullLength - 1;
        
        while (origIdx >= 0)
        {
            uint8_t byte = servoTxBuffer[origIdx--];
            switch (byte)
            {
                case END_PACKET_SYMBOL:
                    servoTxBuffer[fullIdx--] = ESC_END_REPLACE_SYMBOL;
                    servoTxBuffer[fullIdx--] = ESC_SYMBOL;
                
                    break;
                
                case ESC_SYMBOL:
                    servoTxBuffer[fullIdx--] = ESC_ESC_REPLACE_SYMBOL;
                    servoTxBuffer[fullIdx--] = ESC_SYMBOL;
                
                    break;
                
                default:
                    servoTxBuffer[fullIdx--] = byte;
                
                    break;
            }
        }
    }
    
    // Add END_PACKET_SYMBOL to the end of buffer
    servoTxBuffer[fullLength++] = END_PACKET_SYMBOL;
        
    HAL_UART_Transmit_IT(&huart2, servoTxBuffer, fullLength);
}

/* USART2 init function */
void SP_USART2_UART_Init (void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 1000000;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);
}

/**
* @brief This function handles USART2 global interrupt.
*/
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}
