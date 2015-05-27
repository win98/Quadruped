#include "ServoProtocol.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include <stdbool.h>

uint8_t rxBuffer[RX_BUFFER_SIZE];
uint32_t rxCounter = 0;

bool lock_buffer = false;

void(*ev_callback)(SERVO_PROTOCOL_Event*);

void SERVO_PROTOCOL_RcvCallback(uint8_t data);

void SERVO_PROTOCOL_Init (void(**rxCallback)(uint8_t), void(*eventCallback)(SERVO_PROTOCOL_Event*))
{
    *rxCallback = SERVO_PROTOCOL_RcvCallback;
    ev_callback = eventCallback;
}

void SERVO_PROTOCOL_ProcessData(void)
{
    SERVO_PROTOCOL_Event ev;
    
    if (rxCounter >= MIN_VALID_DATA_LENGTH)
    {
        ev.data = rxBuffer;
        ev.dataLength = rxCounter;
        
        ev_callback(&ev);
    }
}

void SERVO_PROTOCOL_RcvCallback(uint8_t data)
{
    if (lock_buffer)
    {
        return;
    }

    switch (data)
    {
        case END_PACKET_SYMBOL:
            lock_buffer = true;
            SERVO_PROTOCOL_ProcessData();
            lock_buffer = false;
            rxCounter = 0;
            return;
        case ESC_SYMBOL:
            return;
        case ESC_END_REPLACE_SYMBOL:
            data = END_PACKET_SYMBOL;
            break;
        case ESC_ESC_REPLACE_SYMBOL:
            data = ESC_SYMBOL;
            break;
        default:
            break;
    }
    
    rxBuffer[rxCounter++] = data;
}
