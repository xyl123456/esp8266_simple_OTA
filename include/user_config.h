#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "espressif/esp_common.h"
#include "espressif/esp_system.h"
#include "uart.h"
#include "gpio.h"



#define UART_GPRS() GPIO_OUTPUT_SET(4, 0) //APONTA GPRS
#define UART_METER() GPIO_OUTPUT_SET(4, 1) //APONTA MEDIDOR
#define GPRS_RESET_H() GPIO_OUTPUT_SET(5, 1)
#define GPRS_RESET_L() GPIO_OUTPUT_SET(5, 0)

#define DEBUG_WRITER(X) printf(X)
#define DEBUG_WRITER_1P(X,Y) printf(X,Y)
#define DEBUG_WRITER_2P(X,A,B) printf(X,A,B)
#define DEBUG_WRITER_4P(X,A,B,C,D) printf(X,A,B,C,D)

enum module_op_mode{MQTT_OFF, MQTT_GPRS, MQTT_WIFI};

struct global_struct {
	uint8_t save_head;
	int global_conf_size;
	uint8_t wifi_serial_as_ssid_n_pass:1;
	WIFI_MODE wifi_mode;
	uint8_t ap_mode_hidden;
	char wifi_ssid_ap[32];
	char wifi_password_ap[32];
	char wifi_ssid_st[10][32];
	char wifi_password_st[10][32];
	char module_serial[16];
	enum module_op_mode op_mode;
	uint8_t read_ansi_serial;
	uint8_t client_serial[17];
	uint8_t client_key[32];
	uint8_t cksum;
};
#define delay_100mili (portTickType) (100 / portTICK_RATE_MS)
#define delay_500mili (portTickType) (500 / portTICK_RATE_MS)
extern struct global_struct GLOBAL_CONF;
#endif
