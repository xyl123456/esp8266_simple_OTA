/*
 * config_server.c
 *
 *  Created on: 15/02/2016
 *      Author: renan
 */


#include "config_server.h"

#include <espressif/espconn.h>
static struct espconn config_server_conn;
static esp_tcp config_server_tcp;

#define COMMAND_ST_USER_PASS 'A'
#define COMMAND_SERIAL_ESP 'N'
#define COMMAND_SERIAL_METER 'M'
#define COMMAND_TRY_ST 'B'
#define COMMAND_SOFTWARE_VERSION 'V'
#define COMMAND_VERIFY_MQTT 'T'

/*
 * Os comandos "COMMAND_UPGRADE" e "COMMAND_SOFTWARE_NOW" são vitais para funcionamento do OTA,
 *
 * Qualquer alteração neles resultará numa possivel alteração no script python "espota.py"
 * que é utilizado pelo makefile
 *
 * */
#define COMMAND_UPGRADE 'U'
#define COMMAND_SOFTWARE_NOW 'S'
#define COMMAND_SOFTWARE_INVERT_USERBIN 'X'
//ATime Energy<NUL><NUL><NUL><NUL><NUL><NUL><NUL><NUL><NUL>@cessorestrito!<NUL><NUL><NUL><NUL><NUL>
//B


struct espconn *upgrade_conn;
uint32_t pos = 0;
uint32_t offset = 0;
static void ICACHE_FLASH_ATTR config_server_recv(void *arg, char *data, unsigned short len) {
	struct espconn *conn = arg;
		if(data[0] == 'F' && len == 1){
			system_upgrade_flag_set(2);
			system_upgrade_reboot();
		}else if(data[0] == '+' && len != 513){
			upgrade_conn = 0;
			pos = 0;
			offset = 0;
			data[0] = 'E';
			data[1] = 'R';
			data[2] = 'R';
			data[3] = 'O';
			data[4] = 'R';
			data[5] = ' ';
			data[6] = '1';
			data[7] = '\0';
			espconn_send(conn, data,8);
		}else if(data[0] == '+' && len == 513){
			if((offset+pos) % 4096 == 0){
				SpiFlashOpResult result = spi_flash_erase_sector((offset+pos) / 4096);
				if(result != SPI_FLASH_RESULT_OK){
					upgrade_conn = 0;
					pos = 0;
					offset = 0;
					data[0] = 'E';
					data[1] = 'R';
					data[2] = 'R';
					data[3] = 'O';
					data[4] = 'R';
					data[5] = ' ';
					data[6] = '2';
					data[7] = '\0';
					espconn_send(conn, data,8);
				}
			}
			int i = 0;
			for(i=0;i<512;i++)
				data[i] = data[i+1];

			if(spi_flash_write(offset+pos,(uint32*) data,512) == SPI_FLASH_RESULT_OK){
				data[0] = 'O';
				data[1] = 'K';
				data[2] = '\0';
				pos+=512;
				espconn_send(conn, data,3);
				return;
			}else{
				data[0] = 'E';
				data[1] = 'R';
				data[2] = 'R';
				data[3] = 'O';
				data[4] = 'R';
				data[5] = ' ';
				data[6] = '3';
				data[7] = '\0';
				espconn_send(conn, data,8);
			}
		}
	else if(data[0] == COMMAND_ST_USER_PASS){
		//memcpy(GLOBALS.user_st, data+1, 20);
		//memcpy(GLOBALS.password_st, data+21, 20);
		data[0] = 15;
		espconn_send(conn, data,1);
	}else if(data[0] == COMMAND_SERIAL_ESP){
		espconn_send(conn, GLOBAL_CONF.module_serial,16);
	}else if(data[0] == COMMAND_TRY_ST){
		//GLOBALS.try_st = 1;
		data[0] = 15;
		espconn_send(conn, data,1);
	}else if(data[0] == COMMAND_UPGRADE){
		upgrade_conn = conn;
		pos = 0;
		if(system_upgrade_userbin_check() == 0)
			offset = 0x101000;//516*1024;
		else
			offset = 4*1024;
		return;
	}else if(data[0] == COMMAND_SOFTWARE_NOW){
		int result = system_upgrade_userbin_check();
		data[0] = result+0x30;
		espconn_send(conn, data,1);
	}else if(data[0] == COMMAND_SOFTWARE_VERSION){
		data[0] = 'A';
		espconn_send(conn, data,1);
	}
	espconn_disconnect(conn);
}


static void ICACHE_FLASH_ATTR config_server_connectedcb(void *arg) {
	struct espconn *conn = arg;
	espconn_regist_recvcb(conn, config_server_recv);
}

LOCAL int initialized = 0;

void config_server_init(int port){
	if(initialized == 1)
		return;
	initialized = 1;
	config_server_conn.type=ESPCONN_TCP;
	config_server_conn.state=ESPCONN_NONE;
	config_server_tcp.local_port=port;
	config_server_conn.proto.tcp=&config_server_tcp;
	upgrade_conn = 0;
	espconn_regist_connectcb(&config_server_conn, &config_server_connectedcb);
	espconn_accept(&config_server_conn);
	espconn_regist_time(&config_server_conn, 900000, 0);

}



