/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "espressif/esp_common.h"
#include "espressif/esp_system.h"


#include "user_config.h"
#include "freertos/queue.h"
#include "gpio.h"

static struct ip_info ipConfig;
unsigned char *p = (unsigned char*)&ipConfig.ip.addr;
struct global_struct GLOBAL_CONF;

#define WIFI_CLIENTSSID		"Time Energy"
#define WIFI_CLIENTPASSWORD	"@cessorestrito!"

//#define WIFI_CLIENTSSID		"LHC"
//#define WIFI_CLIENTPASSWORD	"tijolo22"

void ICACHE_FLASH_ATTR wifiConnectCb(System_Event_t *evt)
{
	printf("Wifi event: %d\r\n", evt->event_id);
	switch (evt->event_id) {
	case EVENT_STAMODE_CONNECTED:
		printf("connected to ssid %s, channel %d\n",
				evt->event_info.connected.ssid,
				evt->event_info.connected.channel);


		break;
	case EVENT_STAMODE_DISCONNECTED:
		printf("disconnected from ssid %s, reason %d\n",
				evt->event_info.disconnected.ssid,
				evt->event_info.disconnected.reason);
		break;
	case EVENT_STAMODE_AUTHMODE_CHANGE:
		printf("mode: %d -> %d\n",
				evt->event_info.auth_change.old_mode,
				evt->event_info.auth_change.new_mode);
		break;
	case EVENT_STAMODE_GOT_IP:
		wifi_get_ip_info(STATION_IF, &ipConfig);
		printf("%d.%d.%d.%d\n\n",p[0],p[1],p[2],p[3]);
//		char ipBuf[] = {192, 168, 0, 109};
//		handleUpgrade(2, ipBuf, 8000, "/");
		break;
	case EVENT_SOFTAPMODE_STACONNECTED:
		printf("station: " MACSTR "join, AID = %d\n",
				MAC2STR(evt->event_info.sta_connected.mac),
				evt->event_info.sta_connected.aid);
		break;
	case EVENT_SOFTAPMODE_STADISCONNECTED:
		printf("station: " MACSTR "leave, AID = %d\n",
				MAC2STR(evt->event_info.sta_disconnected.mac),
				evt->event_info.sta_disconnected.aid);
		break;
	default:
		break;
	}
}


void user_rf_pre_init(void){

}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	struct station_config stconfig;

	uartQueue = xQueueCreate(500, 1);
//	UART_SetBaudrate(UART0, BIT_RATE_9600);
//	UART_ConfigTypeDef uart_config;
//	uart_config.baud_rate    = BIT_RATE_74880;
//	uart_config.data_bits     = UART_WordLength_8b;
//	uart_config.parity          = USART_Parity_None;
//	uart_config.stop_bits     = USART_StopBits_1;
//	uart_config.flow_ctrl      = USART_HardwareFlowControl_None;
//	uart_config.UART_RxFlowThresh = 120;
//	uart_config.UART_InverseMask = UART_None_Inverse;
//	UART_ParamConfig(UART0, &uart_config);
//	UART_SetPrintPort(UART0);
	espconn_init();
	DEBUG_WRITER_1P("SDK version:%s\r\n", system_get_sdk_version());
	//UART_SetPrintPort(UART0);

	wifi_set_opmode(STATION_MODE);
	if(wifi_station_get_config(&stconfig))
	{
		memcpy(&stconfig.ssid, WIFI_CLIENTSSID, sizeof(WIFI_CLIENTSSID));
		memcpy(&stconfig.password, WIFI_CLIENTPASSWORD, sizeof(WIFI_CLIENTPASSWORD));
		wifi_station_set_config(&stconfig);
		printf("SSID: %s\n",stconfig.ssid);
	}

	wifi_set_event_handler_cb(wifiConnectCb);
	config_server_init(24);
}
