/*
	The hello world demo
 */

#include <ets_sys.h>
#include <osapi.h>

#include "user_interface.h"
#include <os_type.h>
#include <gpio.h>
#include "driver/uart.h"
#include "mem.h"
#include <upgrade.h>
#include <ip_addr.h>
#include "espconn.h"



//#define WIFI_CLIENTSSID		"Time Energy"
//#define WIFI_CLIENTPASSWORD	"@cessorestrito!"

#define WIFI_CLIENTSSID		"LHC"
#define WIFI_CLIENTPASSWORD	"tijolo22"

extern int ets_uart_printf(const char *fmt, ...);
static struct ip_info ipConfig;
unsigned char *p = (unsigned char*)&ipConfig.ip.addr;


static void ICACHE_FLASH_ATTR ota_finished_callback(void *arg)
{
	struct upgrade_server_info *update = arg;
	if (update->upgrade_flag == true)
	{
		ets_uart_printf("[OTA]success; rebooting!\n");
		system_upgrade_reboot();
	}
	else
	{
		ets_uart_printf("[OTA]failed!\n");
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
	case UPGRADE_FW_BIN1: file = "user2.512.new.bin"; break;
	case UPGRADE_FW_BIN2: file = "user1.512.new.bin"; break;
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

	ets_uart_printf("[OTA]Server "IPSTR":%d. Path: %s%s\n", IP2STR(update->ip), update->port, path, file);

	update->check_cb = ota_finished_callback;
	update->check_times = 10000;
	update->url = "GET /upgrade/user2.512.new.bin HTTP/1.1";

//	ets_uart_printf("GET %s%s HTTP/1.1\r\n"
//			"Host: "IPSTR":%d\r\n"
//			"Connection: close\r\n"
//			"\r\n",
//			path, file, IP2STR(update->ip), update->port);
	ets_uart_printf("URL: %s.\r\n", (char*)update->url);

	if (system_upgrade_start(update) == false)
	{
		os_printf("[OTA]Could not start upgrade\n");

		os_free(update->pespconn);
		os_free(update->url);
		os_free(update);
	}
	else
	{
		os_printf("[OTA]Upgrading...\n");
	}
}

char ipBuf[] = {192, 168, 50, 85};

void ICACHE_FLASH_ATTR wifiConnectCb(System_Event_t *evt)
{
	uint8_t i;
	bool res = false;
	ets_uart_printf("Wifi event: %d\r\n", evt->event);
	if(evt->event == EVENT_STAMODE_GOT_IP){
		wifi_get_ip_info(STATION_IF, &ipConfig);
		ets_uart_printf("%d.%d.%d.%d",p[0],p[1],p[2],p[3]);
		handleUpgrade(2, ipBuf, 80, "/upgrade/");
	}
	ets_uart_printf("Free heap: %d\r\n", system_get_free_heap_size());
}

void user_rf_pre_init(void)
{
}

void user_init(void)
{
	struct station_config stconfig;

	// Configure the UART
	uart_init(BIT_RATE_74880, BIT_RATE_74880);
	ets_uart_printf("SDK version:%s rom %d\n", system_get_sdk_version(), system_upgrade_userbin_check());
	ets_uart_printf("Nova versão user2");

	wifi_set_opmode(STATION_MODE);
	if(wifi_station_get_config(&stconfig))
	{
		os_memcpy(&stconfig.ssid, WIFI_CLIENTSSID, sizeof(WIFI_CLIENTSSID));
		os_memcpy(&stconfig.password, WIFI_CLIENTPASSWORD, sizeof(WIFI_CLIENTPASSWORD));
		wifi_station_set_config(&stconfig);
		ets_uart_printf("SSID: %s\n",stconfig.ssid);
	}

	wifi_set_event_handler_cb(wifiConnectCb);

	wifi_station_set_auto_connect(TRUE);
	wifi_station_set_reconnect_policy(1);
	wifi_station_connect();
}
