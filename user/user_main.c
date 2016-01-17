/*
	The hello world demo
 */

#include <ets_sys.h>
#include <osapi.h>

#include <user_interface.h>
#include <os_type.h>
#include <gpio.h>
#include "../include/driver/uart.h"
#include "../include/driver/uart_register.h"
#include "../include/driver/crc.h"
#include "mem.h"
#include <upgrade.h>
#include <ip_addr.h>
#include "espconn.h"

//python C:\Espressif\utils\ota.py 192.168.0.104 D:\IOT\ESP8266\Projetos_Server\Simple_OTA_test\fw\user2.bin


#define UART0   0
#define UART1   1
#define user_procTaskPrio        0
#define user_procTaskQueueLen    1


#define WIFI_CLIENTSSID		"Time Energy"
#define WIFI_CLIENTPASSWORD	"@cessorestrito!"

//#define WIFI_CLIENTSSID		"LHC"
//#define WIFI_CLIENTPASSWORD	"tijolo22"


os_event_t    user_procTaskQueue[user_procTaskQueueLen];
extern int ets_uart_printf(const char *fmt, ...);
static struct ip_info ipConfig;
unsigned char *p = (unsigned char*)&ipConfig.ip.addr;
uint8 memory_map[128];
uint32 i;
uint16 crc_fw=0;

static ETSTimer WiFiLinker;

//Main code function
static void ICACHE_FLASH_ATTR int_rx(os_event_t *events) {

  int c = uart0_rx_one_char();

  if(c == '1') {
		if(system_upgrade_userbin_check()==1){
			ets_uart_printf("Boot Rom 1\n");
			system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
			system_upgrade_reboot();
		}
  }

}

void ICACHE_FLASH_ATTR timer0(void *arg){
	os_timer_disarm(&WiFiLinker);
	ets_uart_printf("Timer\n");
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)timer0, NULL);
	os_timer_arm(&WiFiLinker, 2000, 0);
}

static void ICACHE_FLASH_ATTR ota_finished_callback(void *arg)
{
	struct upgrade_server_info *update = arg;
	ets_uart_printf("\n\n Upgradeflag 0x%02x ",update->upgrade_flag);
	if (update->upgrade_flag == true)
	{
		ets_uart_printf("[OTA]success; rebooting!\n");
		system_upgrade_reboot();
	}
	else
	{
		ets_uart_printf("[OTA]failed!\n");
		crc_fw=0;
		for(i=0x41000;i<=0x7c000;i++){
			system_soft_wdt_feed();
			spi_flash_read(i,(uint32*)memory_map,1);
			crc_fw = update_crc_16(crc_fw,memory_map[0]);
		}
		ets_uart_printf("CRC do firmware user bin2 é 0x%04x \n\n",crc_fw);
//		spi_flash_read(0x41000,(uint32*)memory_map,128);
//		ets_uart_printf("----flag set----\n");
//		for(i=0;i<128;i++){
//			ets_uart_printf("0x%02x ",memory_map[i]);
//		}
//		ets_uart_printf("\n-------------------------------\n\n");

	}

	os_free(update->pespconn);
	os_free(update->url);
	os_free(update);
}

static void ICACHE_FLASH_ATTR handleUpgrade(uint8_t serverVersion, const char *server_ip, uint16_t port, const char *path)
{
	const char* file;
	uint8_t userBin = system_upgrade_userbin_check();
	switch (userBin)
	{
	case UPGRADE_FW_BIN1: file = "user2.bin"; break;
	case UPGRADE_FW_BIN2: file = "user1.bin"; break;
	default: ets_uart_printf("[OTA]Invalid userbin number!\n");
	return;
	}

	uint16_t version=1;
	if (serverVersion <= version)
	{
		ets_uart_printf("[OTA]No update. Server version:%d, local version %d\n", serverVersion, version);
		return;
	}

	ets_uart_printf("[OTA]Upgrade available version: %d\n", serverVersion);

	struct upgrade_server_info* update = (struct upgrade_server_info *)os_zalloc(sizeof(struct upgrade_server_info));
	update->pespconn = (struct espconn *)os_zalloc(sizeof(struct espconn));

	os_memcpy(update->ip, server_ip, 4);
	update->port = port;

	ets_uart_printf("[OTA]Server "IPSTR":%d. Path: %s%s\n\n", IP2STR(update->ip), update->port, path, file);

	update->check_cb = ota_finished_callback;
	update->check_times = 30000;
	update->url = (uint8 *)os_zalloc(512);

	os_sprintf((char*)update->url,
			"GET %s%s HTTP/1.0\r\n"
			"Host: "IPSTR":%d\r\n"
			"Connection: keep-alive\r\n Cache-Control: no-cache\r\n"
			"\r\n",
			path, file, IP2STR(update->ip), update->port);
	ets_uart_printf("URL: %s.\r\n", (char*)update->url);

	if (system_upgrade_start(update) == false)
	{
		ets_uart_printf("[OTA]Could not start upgrade\n");

		os_free(update->pespconn);
		os_free(update->url);
		os_free(update);
	}
	else
	{
		ets_uart_printf("[OTA]Upgrading...\n");
	}
}



void ICACHE_FLASH_ATTR wifiConnectCb(System_Event_t *evt)
{
	ets_uart_printf("Wifi event: %d\r\n", evt->event);
	switch (evt->event) {
	case EVENT_STAMODE_CONNECTED:
		ets_uart_printf("connected to ssid %s, channel %d\n",
				evt->event_info.connected.ssid,
				evt->event_info.connected.channel);


		break;
	case EVENT_STAMODE_DISCONNECTED:
		ets_uart_printf("disconnected from ssid %s, reason %d\n",
				evt->event_info.disconnected.ssid,
				evt->event_info.disconnected.reason);
		break;
	case EVENT_STAMODE_AUTHMODE_CHANGE:
		ets_uart_printf("mode: %d -> %d\n",
				evt->event_info.auth_change.old_mode,
				evt->event_info.auth_change.new_mode);
		break;
	case EVENT_STAMODE_GOT_IP:
		wifi_get_ip_info(STATION_IF, &ipConfig);
		ets_uart_printf("%d.%d.%d.%d\n\n",p[0],p[1],p[2],p[3]);
//		char ipBuf[] = {192, 168, 0, 109};
//		handleUpgrade(2, ipBuf, 8000, "/");
		break;
	case EVENT_SOFTAPMODE_STACONNECTED:
		ets_uart_printf("station: " MACSTR "join, AID = %d\n",
				MAC2STR(evt->event_info.sta_connected.mac),
				evt->event_info.sta_connected.aid);
		break;
	case EVENT_SOFTAPMODE_STADISCONNECTED:
		ets_uart_printf("station: " MACSTR "leave, AID = %d\n",
				MAC2STR(evt->event_info.sta_disconnected.mac),
				evt->event_info.sta_disconnected.aid);
		break;
	default:
		break;
	}
}

void user_rf_pre_init(void)
{
	uart_init(BIT_RATE_74880, BIT_RATE_74880);
}


void user_init(void)
{

	struct station_config stconfig;


	ets_uart_printf("SDK version:%s rom %d\n", system_get_sdk_version(), system_upgrade_userbin_check());
	crc_fw=0;
	for(i=0x41000;i<=0x7c000;i++){
		system_soft_wdt_feed();
		spi_flash_read(i,(uint32*)memory_map,1);
		crc_fw = update_crc_16(crc_fw,memory_map[0]);
		if(i%4096==0){
			ets_uart_printf("addr:0x%06x crc: 0x%04x \n",i,crc_fw);
		}
	}
	ets_uart_printf("\n\nCRC do firmware user bin2 é 0x%04x \n\n",crc_fw);

	//ets_uart_printf("Boot 2\n");
	wifi_set_opmode(STATION_MODE);
	if(wifi_station_get_config(&stconfig))
	{
		os_memcpy(&stconfig.ssid, WIFI_CLIENTSSID, sizeof(WIFI_CLIENTSSID));
		os_memcpy(&stconfig.password, WIFI_CLIENTPASSWORD, sizeof(WIFI_CLIENTPASSWORD));
		wifi_station_set_config(&stconfig);
		ets_uart_printf("SSID: %s\n",stconfig.ssid);
	}

//	spi_flash_read(0x72680,(uint32*)memory_map,128);
//	ets_uart_printf("----flag set----\n");
//	for(i=0;i<128;i++){
//		ets_uart_printf("0x%02x ",memory_map[i]);
//	}
//	ets_uart_printf("\n-------------------------------\n\n");


	wifi_set_event_handler_cb(wifiConnectCb);
	wifi_station_set_auto_connect(TRUE);
	wifi_station_set_reconnect_policy(1);
	wifi_station_connect();


	os_timer_disarm(&WiFiLinker);
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)timer0, NULL);
	os_timer_arm(&WiFiLinker, 1000, 0);

    system_os_task(int_rx, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
}



