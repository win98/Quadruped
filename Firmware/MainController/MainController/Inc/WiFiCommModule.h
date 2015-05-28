#ifndef _WIFI_COMM_MODULE_H_
#define _WIFI_COMM_MODULE_H_

#include "cmsis_os.h"

typedef enum
{
    WF_TX_ERROR = 1,
    WF_RX_ERROR,
    WF_SET_AP_MODE_ERROR,
    WF_START_AP_ERROR,
    WF_ENABLE_RADIO_ERROR,
    WF_ENABLE_DHCP_ERROR,
    WF_START_UDP_SERVER_ERROR,
    WF_CREATE_CLIENT_EP_ERROR,
    WF_UDP_SEND_ERROR,
    WF_EP_CLOSE_ERROR
} WF_ERROR_Code;

typedef enum
{
    WF_ROBOT_NO_CMD = 0,
    WF_ROBOT_TAKE_CONTROL = '1',
    WF_ROBOT_DROP_CONTROL,
    WF_ROBOT_SET_CHANNEL,
    WF_ROBOT_SET_CHANNELS,
    WF_ROBOT_SET_LINEAR_VELOCITY,
    WF_ROBOT_SET_ANGULAR_VELOCITY,
    WF_ROBOT_PING
} WF_ROBOT_COMMAND;

typedef enum
{
    WF_CMD_OK = '0',
    WF_CMD_ERROR
} WF_COMMAND_STATUS;

typedef struct
{
    WF_ROBOT_COMMAND cmd;
    uint8_t *data;
    uint32_t length;
} WF_Robo_Packet;

void WF_Init (void);
void WF_SetRxQueue (xQueueHandle queue);
void WF_CommThread (void const *argument);
void WF_RobotPacketProcessed (WF_Robo_Packet *datagramInfo);

#endif
