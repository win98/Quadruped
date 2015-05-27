#ifndef _SERVO_PROTOCOL_MASTER_H_
#define _SERVO_PROTOCOL_MASTER_H_

#include <stdint.h>

//Special symbols
#define END_PACKET_SYMBOL           0xC0
#define ESC_SYMBOL                  0xDB
#define ESC_END_REPLACE_SYMBOL      0xDC
#define ESC_ESC_REPLACE_SYMBOL      0xDD

//Servo protocol commans
typedef enum
{
    SERVO_SET_CHANNEL   = 0x01,
    SERVO_SET_CHANNELS  = 0x02
}
SERVO_PROTOCOL_Command;

void SERVO_PROTOCOL_Init (void);
void SERVO_PROTOCOL_SendCommand (SERVO_PROTOCOL_Command cmd, uint8_t channelNum, uint16_t *data, uint32_t dataLength);

#endif