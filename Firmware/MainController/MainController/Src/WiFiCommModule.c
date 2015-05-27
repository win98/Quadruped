#include "WiFiCommModule.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stm32f2xx_hal.h"
#include "wifi_bglib.h"

/*
 * BGLIB additional definitions
 */
#define WIFI_MODE_AP					0x02
#define WIFI_AP_SECURITY_NONE			0x00

#define WIFI_OPTION_ENABLED				0x01
#define WIFI_OPTION_DISABLED			0x00

#define WIFI_OPTION_BOOT_TO_PROGRAM	    0x00

/*
 * Definitions
 */
#define WF_NIL  0

/*
 * Mandatory declaration
 */
BGLIB_DEFINE();

/*
 * HW driver settings
 */
UART_HandleTypeDef huart3;
const uint32_t k_wifi_module_comm_baudrate = 115200;

/*
 * WiFi AP settings
 */
uint8_t * const k_wifi_ap_ssid = "SkyNet";          // AP mode SSID
const uint8_t k_wifi_channel = 11;                  // AP mode channel

/*
 * UDP protocol settings
 */
const uint16_t k_wife_udp_server_port = 49001;      // UDP server port for incoming connections

/* 
 * Internal machine states.
 */
enum wlan_state
{
    // Send Hello command.
    wlan_state_hello,
    // Reset the module.
    wlan_state_reset,
	// Set AP mode.
	wlan_state_set_ap_mode,
    // Switch Wi-Fi on.
    wlan_state_wifi_on,
	// Enable DHCP.
	wlan_state_enable_dhcp,
	// Start AP mode.
	wlan_state_start_ap_mode,
	// Create UDP server endpoint.
	wlan_state_start_udp_server,
	// Create UDP client endpoint.
	wlan_state_start_udp_client,
	// Server received request.
	wlan_state_udp_received_request,
	// Send response.
	wlan_state_udp_send_response,
	// Idle state.
	wlan_state_idle
};

/* 
 * Internal variables.
 */
// State machine current state.
static enum wlan_state internal_state;
// UDP server endpoint.
static uint8_t udp_server_endpoint;
// UDP client endpoint.
static uint8_t current_client_endpoint = WF_NIL;
// Buffer for transmit data.
static uint8_t tx_packet[1024];
// Index to transmit buffer.
static uint16_t tx_packet_length = 0;

// Accepted client IP address.
static uint32_t remote_client_address = WF_NIL;
// Accepted client port.
static uint16_t remote_client_port = WF_NIL;
// Accepted client endpoint.
static uint8_t remote_client_endpoint = WF_NIL;
// Connected client temp IP address.
static uint32_t new_remote_client_address = WF_NIL;
// Connected client temp port.
static uint32_t new_remote_client_port = WF_NIL;
// Robot protocol command from request.
WF_ROBOT_COMMAND robotCmd;

/*
 * Functions prototypes
 */
void parseAddressToString (uint32_t address, char *str);
void WF_USART3_UART_Init (void);
HAL_StatusTypeDef uart_tx (int data_length, uint8_t *data);
HAL_StatusTypeDef uart_rx (int data_length, uint8_t *data);
static void send_message (uint8 msg_len, uint8 *msg_data, uint16 data_len, uint8 *data);
void WF_ERROR_Handler (WF_ERROR_Code error);
void WF_ParseRequest (uint8_t *data, uint32_t dataLength, uint32_t address, uint16_t port);
void WF_ClientEndpointCreated (uint8_t endpoint);

/*
 * Function for switching between states.
 * @param state State to switch to.
 */
static void goto_state (enum wlan_state state)
{
    switch (state)
    {
        case wlan_state_hello:
//          printf("goto_state() - wlan_state_hello\n");
            wifi_cmd_system_hello();

            break;

        case wlan_state_reset:
//          printf("goto_state() - wlan_state_reset\n");
			wifi_cmd_system_reset(WIFI_OPTION_BOOT_TO_PROGRAM);

            break;

		case wlan_state_set_ap_mode:
//			printf("goto_state() - wlan_state_set_ap_mode\n");
			wifi_cmd_sme_set_operating_mode(WIFI_MODE_AP);

			break;

        case wlan_state_wifi_on:
//          printf("goto_state() - wlan_state_wifi_on\n");
            wifi_cmd_sme_wifi_on();

            break;

		case wlan_state_enable_dhcp:
//			printf("goto_state() - wlan_state_enable_dhcp\n");
			wifi_cmd_https_enable(WIFI_OPTION_DISABLED, WIFI_OPTION_ENABLED, WIFI_OPTION_DISABLED);

			break;

		case wlan_state_start_ap_mode:
//			printf("goto_state() - wlan_state_start_ap_mode\n");
			wifi_cmd_sme_start_ap_mode(k_wifi_channel, WIFI_AP_SECURITY_NONE, strlen((char const *)k_wifi_ap_ssid),
                                       k_wifi_ap_ssid);

			break;

		case wlan_state_start_udp_server:
//			printf("goto_state() - wlan_state_start_udp_server\n");
			wifi_cmd_tcpip_start_udp_server(k_wife_udp_server_port, -1);

			break;

		case wlan_state_start_udp_client:
//			printf("goto_state() - wlan_state_start_udp_client\n");
			wifi_cmd_tcpip_udp_connect(new_remote_client_address, new_remote_client_port, -1);

			break;

		case wlan_state_udp_send_response:
//			printf("goto_state() - wlan_state_udp_send_response\n");
			wifi_cmd_endpoint_send(current_client_endpoint, tx_packet_length, tx_packet);

			break;

		case wlan_state_idle:
			break;

        default:
            break;
    }
    internal_state = state;
}

void WF_Init (void)
{    
    //Init HW.    
    WF_USART3_UART_Init();
    
    //Initialize BGLIB with output function for sending messages.
    BGLIB_INITIALIZE(send_message);
}

void WF_CommThread (void)
{
    HAL_StatusTypeDef ret;
    // Pointer to wifi packet.
    struct wifi_cmd_packet *pck;
    // Buffer for storing data from the serial port.
    static unsigned char buffer[BGLIB_MSG_MAXLEN];
    // Length of message payload data.
    uint16_t msg_length;
	// Array for storing IP.
//	char ipAsStr[16];

    // Received data.
    uint8_t *data;
    // Received data length.
    uint32_t dataLength;
    
    // Send a Hello command to verify the serial port is working.
    goto_state (wlan_state_hello);

    // The main program is a loop where most of the time is
    // spent waiting for blocking uart_rx() call.
    while (1)
    {
        // Read enough data from serial port for the BGAPI message header.
        ret = uart_rx (BGLIB_MSG_HEADER_LEN, buffer);
        
        if (ret != HAL_OK)
        {
            WF_ERROR_Handler (WF_RX_ERROR);
        }

        // The buffer now contains the message header. See BGAPI protocol definition for details on packet format.
        msg_length = BGLIB_MSG_LEN (buffer);

        // Read the payload data if required and store it after the header.
        if (msg_length)
        {
            ret = uart_rx (msg_length, &buffer[BGLIB_MSG_HEADER_LEN]);
            
            if (ret != HAL_OK)
            {
                WF_ERROR_Handler (WF_RX_ERROR);
            }
        }
        
        pck = BGLIB_MSG (buffer);
        
        switch (BGLIB_MSG_ID (buffer))
        {
            case wifi_rsp_system_hello_id:
//              printf("on_wifi_rsp_system_hello()\n");

                // BGAPI communication channel works, reset the module.
                if (internal_state == wlan_state_hello)
                {
                    goto_state (wlan_state_reset);
                }

                break;

            case wifi_evt_system_boot_id:
 //             printf("on_wifi_evt_system_boot()\n");

                // Set AP mode.
                goto_state(wlan_state_set_ap_mode);

                break;

			case wifi_rsp_sme_set_operating_mode_id:
//				printf("on_wifi_rsp_sme_set_operating_mode()\n");

				if(pck->rsp_sme_set_operating_mode.result != wifi_err_success)
				{
                    WF_ERROR_Handler (WF_SET_AP_MODE_ERROR);
				}

				// Switch WiFi tranceiver on.
				goto_state(wlan_state_wifi_on);

				break;

			case wifi_rsp_sme_wifi_on_id:
//              printf("on_wifi_rsp_sme_wifi_on()\n");

                if(pck->rsp_sme_wifi_on.result != wifi_err_success)
                {
                    WF_ERROR_Handler (WF_ENABLE_RADIO_ERROR);
                }

                break;

            case wifi_evt_sme_wifi_is_on_id:
//              printf("on_wifi_evt_sme_wifi_is_on()\n");

                if(pck->evt_sme_wifi_is_on.result != wifi_err_success)
                {
                    WF_ERROR_Handler (WF_ENABLE_RADIO_ERROR);
                }

                // Wi-Fi has been successfully switched on, start AP mode.
				goto_state(wlan_state_enable_dhcp);

                break;

			case wifi_rsp_https_enable_id:
//				printf("on_wifi_rsp_https_enable()\n");

				if(pck->rsp_https_enable.result != wifi_err_success)
				{
                    WF_ERROR_Handler (WF_ENABLE_DHCP_ERROR);
				}

				goto_state(wlan_state_start_ap_mode);

				break;

			case wifi_rsp_sme_start_ap_mode_id:
//				printf("on_wifi_rsp_sme_start_ap_mode()\n");

				if(pck->rsp_sme_start_ap_mode.result != wifi_err_success)
				{
                    WF_ERROR_Handler (WF_START_AP_ERROR);
				}

				break;

			case wifi_evt_sme_ap_mode_started_id:
//				printf("on_wifi_evt_sme_ap_mode_started_successfuly()\n");

                // Start UDP server for incoming data.
				goto_state(wlan_state_start_udp_server);

				break;

			case wifi_evt_sme_ap_mode_failed_id:
//				printf("on_wifi_evt_sme_ap_mode_failed_to_start()\n");

                WF_ERROR_Handler (WF_START_AP_ERROR);

				break;

			case wifi_rsp_tcpip_start_udp_server_id:
//				printf("on_wifi_rsp_tcpip_start_udp_server()\n");

				if(pck->rsp_tcpip_start_udp_server.result != wifi_err_success)
				{
                    WF_ERROR_Handler (WF_START_UDP_SERVER_ERROR);
				}
                
                // Save created UDP server endpoint value.
				udp_server_endpoint = pck->rsp_tcpip_start_udp_server.endpoint;

				break;

			case wifi_evt_endpoint_status_id:
                //Event generated after any endpoint created
            
//              printf("on_wifi_evt_endpoint_status(), endpoint: %d, is_streaming: %d, dest_endpoint: %d, is_active: %d\n",
//                      pck->evt_endpoint_status.endpoint,
//                      pck->evt_endpoint_status.streaming,
//                      pck->evt_endpoint_status.destination,
//                      pck->evt_endpoint_status.active);

				if (pck->evt_endpoint_status.endpoint == udp_server_endpoint 
                    && internal_state == wlan_state_start_udp_server)
                {
                    // Server endpoint is created. Do nothing.
                    
					goto_state(wlan_state_idle);
                }
				else if (pck->evt_endpoint_status.endpoint == current_client_endpoint)
				{
                    // Client endpoint is created as it was requested by remote client.
                    
					WF_ClientEndpointCreated (pck->evt_endpoint_status.endpoint);
				}

                break; 

			case wifi_rsp_endpoint_send_id:
                // UDP server sent data packet
            
//				printf("on_wifi_rsp_endpoint_send()\n");

				if(pck->rsp_endpoint_send.result != wifi_err_success)
				{
                    WF_ERROR_Handler (WF_UDP_SEND_ERROR);
				}

                // Close all non-remote client endpoints
                if (current_client_endpoint != remote_client_endpoint)
                {
                    wifi_cmd_endpoint_close(current_client_endpoint);
                }
                else
                {
                    current_client_endpoint = WF_NIL;
                    
                    if (robotCmd == WF_ROBOT_DROP_CONTROL)
                    {
                        wifi_cmd_endpoint_close(remote_client_endpoint);
                    }
                }
                
                robotCmd = WF_ROBOT_NO_CMD;
                goto_state(wlan_state_idle);
                
				break;
                
            case wifi_rsp_endpoint_close_id:
                //Endpoint closed
                
//				printf("wifi_rsp_endpoint_close()\n");
            
                if (pck->rsp_endpoint_close.result != wifi_err_success)
                {
                    WF_ERROR_Handler (WF_EP_CLOSE_ERROR);
                }
                
                if (pck->rsp_endpoint_close.endpoint == current_client_endpoint)
                {
                    current_client_endpoint = WF_NIL;
                }
                else if (pck->rsp_endpoint_close.endpoint == remote_client_endpoint)
                {
                    remote_client_endpoint = WF_NIL;
                    remote_client_address = WF_NIL;
                    remote_client_port = WF_NIL;
                }
                
                break;
                
			case wifi_evt_tcpip_udp_data_id:
                // UDP server received data packet.
            
//				parseAddressToString(pck->evt_tcpip_udp_data.source_address.u, ipAsStr);
//				printf("on_wifi_evt_tcpip_udp_data(), endpoint: %d, source address: %s, source port: %d, length: %d", 
//					 pck->evt_tcpip_udp_data.endpoint, 
//                     ipAsStr, 
//					 pck->evt_tcpip_udp_data.source_port, 
//					 pck->evt_tcpip_udp_data.data.len);

                // Robot protocol logic.
				// Parse data.
                data = pck->evt_tcpip_udp_data.data.data;
                dataLength = pck->evt_tcpip_udp_data.data.len;
                
                if (dataLength >= 1)
                {
                   WF_ParseRequest (data, dataLength, pck->evt_tcpip_udp_data.source_address.u, 
                                                                                  pck->evt_tcpip_udp_data.source_port);
                }
                else
                {
                    goto_state(wlan_state_idle);
                }

				break;

			case wifi_rsp_tcpip_udp_connect_id:
                // UDP client endpoint created
            
//				printf("on_wifi_rsp_tcpip_udp_connect()\n");

				if(pck->rsp_tcpip_udp_connect.result != wifi_err_success)
				{
                    WF_ERROR_Handler (WF_CREATE_CLIENT_EP_ERROR);
				}

				current_client_endpoint = pck->rsp_tcpip_udp_connect.endpoint;

				break;

            default:
                break;

        }
    }
}

/*
 * Function called when a data packet received via WiFi
 * @param data Data packet
 * @param dataLength Data length.
 */
void WF_ParseRequest (uint8_t *data, uint32_t dataLength, uint32_t address, uint16_t port)
{
    robotCmd = (WF_ROBOT_COMMAND)data[0];
    
    uint16_t *servoChannelsData;
    int8_t servoChannelNum;
    
    switch (robotCmd)
    {
        case WF_ROBOT_TAKE_CONTROL:
            //Client requests to take control on robot.
        
            //Check if similar request from
            //any remote client is processing.
            if(new_remote_client_address != WF_NIL)
            {
                // Some connection already in progress. Do nothing.
                
                goto_state(wlan_state_idle);
            }
            else
            {
                // Create udp client endpoint to communicate with remote client.

                new_remote_client_address = address;
                new_remote_client_port = port;
                
                goto_state(wlan_state_start_udp_client);
            }

            break;            

        case WF_ROBOT_DROP_CONTROL:
            //Client requests to drop control on robot.
        
            if (address == remote_client_address && port == remote_client_port)
            {
                // Send response Ok.
                tx_packet[0] = WF_CMD_OK;
                tx_packet_length = 1;
                
                current_client_endpoint = remote_client_endpoint;
                
                goto_state(wlan_state_udp_send_response);
            }
            else
            {
                goto_state(wlan_state_idle);
            }
        
            break;
        
        case WF_ROBOT_SET_CHANNEL:
            //Client requests to set servo channel value.
        
            if (address == remote_client_address && port == remote_client_port)
            {
                servoChannelNum = (int8_t)data[1];
                servoChannelsData = (uint16_t *)&data[2];
                
                //Data comes in Big-endian format.
                //Convert it to Little-endian.
                uint16_t pulseWidthMs = (servoChannelsData[0] & 0x00FF) << 8;
                pulseWidthMs |= (servoChannelsData[0] & 0xFF00) >> 8;
                
                //Send message to main thread with value and channel
                //!!!!!!!!!!!!!
                
                // Send response Ok.
                tx_packet[0] = WF_CMD_OK;
                tx_packet_length = 1;
                
                current_client_endpoint = remote_client_endpoint;
                
                goto_state(wlan_state_udp_send_response);
            }
            else
            {
                goto_state(wlan_state_idle);
            }
            
            break;
            
        default:
            // Do nothing.
            goto_state(wlan_state_idle);
            
            break;
    }
}

/*
 * Function called when client endpoint created.
 * @param endpoint Created client endpoint.
 */
void WF_ClientEndpointCreated (uint8_t endpoint)
{
    if (robotCmd == WF_ROBOT_TAKE_CONTROL)
    {
        // Check if any client already took control.
        if(remote_client_endpoint == WF_NIL)
        {
            // Accept remote client to take control
            remote_client_address = new_remote_client_address;
            remote_client_port = new_remote_client_port;
            remote_client_endpoint = endpoint;

            // Send response Ok.
            tx_packet[0] = WF_CMD_OK;
            tx_packet_length = 1;
        }
        else
        {
            // Send response Fail with current
            // remote cliend data.
            tx_packet[0] = WF_CMD_FAIL;
            tx_packet[1] = (uint8_t)(remote_client_address >> 24);
            tx_packet[2] = (uint8_t)(remote_client_address >> 16);
            tx_packet[3] = (uint8_t)(remote_client_address >> 8); 
            tx_packet[4] = (uint8_t)remote_client_address;
            tx_packet[5] = (uint8_t)(remote_client_port >> 8);
            tx_packet[6] = (uint8_t)remote_client_port;
            tx_packet_length = 7;
        }
        
        // Reset temp variables.
        new_remote_client_address = WF_NIL;
        new_remote_client_port = WF_NIL;
        
        current_client_endpoint = endpoint;
        
        goto_state(wlan_state_udp_send_response);
    }
    else
    {
        wifi_cmd_endpoint_close(endpoint);
        
        goto_state(wlan_state_idle);
    }
}

/*
 * Function called when any error occured.
 */
void WF_ERROR_Handler (WF_ERROR_Code error)
{
    //Send error message to main thread.
    //!!!!!!!!!!!Implement it
    
    // Endless loop.
    while (1);
}

/*
 * Function called when a message needs to be written to the serial port.
 * @param msg_len Length of the message.
 * @param msg_data Message data, including the header.
 * @param data_len Variable data length.
 * @param data Variable data.
 */
static void send_message(uint8 msg_len, uint8* msg_data, uint16 data_len, uint8* data)
{
    /** Variable for storing function return values. */
    HAL_StatusTypeDef ret;

    ret = uart_tx(msg_len, msg_data);
    if(ret != HAL_OK)
    {
        WF_ERROR_Handler(WF_TX_ERROR);
    }

    if(data_len && data)
    {
        ret = uart_tx(data_len, data);
        if(ret != HAL_OK)
        {
            WF_ERROR_Handler(WF_TX_ERROR);
        }
    }
}

/*
 * Function for parsing IP address to string.
 * @param address IP address,
 * @param str String to store.
 */
void parseAddressToString(uint32_t address, char *str)
{
	sprintf(str, "%d.%d.%d.%d",
		(uint8_t)(address),
		(uint8_t)(address >> 8),
		(uint8_t)(address >> 16),
		(uint8_t)(address >> 24));
}

/*
 * Function for HW initializing.
 */
void WF_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = k_wifi_module_comm_baudrate;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);
}

/*
 * Function called when a message needs to be written to the serial port.
 * @param data_length Length of the message.
 * @param data Message data, including the header.
 */
HAL_StatusTypeDef uart_tx(int data_length, uint8_t *data)
{
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart3, data, data_length, 1000000);
    
    return status;
}

/*
 * Function called when a message needs to be read from the serial port.
 * @param data_length Length of the message.
 * @param data Message data, including the header.
 */
HAL_StatusTypeDef uart_rx(int data_length, uint8_t *data)
{
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart3, data, data_length, 1000000);
    
    return status;
}
