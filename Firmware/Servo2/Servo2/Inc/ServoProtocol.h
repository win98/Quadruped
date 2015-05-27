#ifndef _SERVO_PROTOCOL_H_
#define _SERVO_PROTOCOL_H_

#include <stdint.h>

//Rx buffer size
#define RX_BUFFER_SIZE              1024

#define MIN_VALID_DATA_LENGTH       1

//Special symbols
#define END_PACKET_SYMBOL           0xC0
#define ESC_SYMBOL                  0xDB
#define ESC_END_REPLACE_SYMBOL      0xDC
#define ESC_ESC_REPLACE_SYMBOL      0xDD

typedef enum
{
    SERVO_SET_CHANNEL   = 0x01,
    SERVO_SET_CHANNELS  = 0x02
}
SERVO_PROTOCOL_Command;

typedef struct
{
    uint8_t *data;
    uint32_t dataLength;
}
SERVO_PROTOCOL_Event;

void SERVO_PROTOCOL_Init (void(**rxCallback)(uint8_t), void(*eventCallback)(SERVO_PROTOCOL_Event*));

#endif
