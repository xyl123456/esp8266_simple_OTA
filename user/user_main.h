/*
 * user_main.h
 *
 *  Created on: 12/02/2016
 *      Author: renan
 */

#ifndef USER_USER_MAIN_H_
#define USER_USER_MAIN_H_

void start_serial_to_udp();

#define MAX_TXBUFFER 1024
#define MAX_UARTBUFFER (MAX_TXBUFFER/4)

extern uint8 uartbuffer[MAX_UARTBUFFER];


#endif /* USER_USER_MAIN_H_ */
