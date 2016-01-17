/*
 * crc.c
 *
 *  Created on: 22/12/2015
 *      Author: Leandro
 */



 /*****************************************************************************
* © 2015 Time Energy Ltda. Todos os direitos reservados.
*
* ATENÇÃO: Este arquivo possui informações confidenciais e não pode ser
* reproduzido ou distribuído(total ou parcial) , sem a previa autorização
* por escrito da Time Energy.
*****************************************************************************/

/***************************************************************************
 * @file 	lib_crc.c
 * @brief	Biblioteca para calcular do crc
 * @author	Renan
 * @date	23/09/2014
 * @company	Time Energy
 *
 * {Arquivos de Include}
****************************************************************************/

#include "../include/driver/crc.h"


#define                 P_16        0xA001
static int              crc_tab16_init          = FALSE;
static int              crc_ansi_init      = FALSE;
static unsigned short   crc_tab16[256];
static unsigned short   crc_ansi[256];


/*******************************************************************
FUNÇÃO: void init_crc_ansi_tab()

PARÂMETROS:


* DESCRIÇÃO : Inicializa tabela para calculo de crc do protocolo ANSI C12.18
*
* RETORNO :  Nada (void)
*
********************************************************************/

static void             init_crc_ansi_tab( void ){

	unsigned short counter_ii = 0;
	unsigned char counter_j = 0;
	for(counter_ii=0;counter_ii<256;counter_ii++){
		unsigned short temp_crc = 0;
		unsigned short temp_c = counter_ii;
		for(counter_j=0;counter_j<8;counter_j++){
			if((temp_crc ^ temp_c) & 1)
				temp_crc = (temp_crc >>1) ^ 0x8408;
			else
				temp_crc >>=1;
			temp_c >>=1;
		}
		crc_ansi[counter_ii] = temp_crc;

	}
	crc_ansi_init = TRUE;
}
/*******************************************************************
FUNÇÃO: unsigned short update_crc_ansi(unsigned short old, char byte, char CONTROL)

PARÂMETROS:
			unsigned short old: Valor atual do CRC
			char byte: byte a ser utilizado na atualização do CRC
			char CONTROL: controle do calculo, se 0 significa um byte esta sendo adicionado ao calculo,
											   se 1 significa inicio do calculo
											   se 2 significa fim do calculo

* DESCRIÇÃO : realiza o calculo do crc
*
* RETORNO :  crc calculado
*
********************************************************************/
unsigned short update_crc_ansi(unsigned short old, char byte, char CONTROL){
	if ( ! crc_ansi_init ) init_crc_ansi_tab();

	if(CONTROL == 1)
		old = 0xFFFF;

	old = (old>>8) ^ crc_ansi[(old^byte)&0xFF];

	if(CONTROL == 2)
		old = old ^0xFFFF;

	return old;
}

/*******************************************************************
FUNÇÃO: void init_crc16_tab()

PARÂMETROS:


* DESCRIÇÃO : Inicializa tabela para calculo de crc do protocolo ABNT NBR14522
*
* RETORNO :  Nada (void)
*
********************************************************************/
static void init_crc16_tab( void ) {

	int i, j;
	unsigned short crc, c;

	for (i=0; i<256; i++) {

		crc = 0;
		c   = (unsigned short) i;

		for (j=0; j<8; j++) {

			if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_16;
			else                      crc =   crc >> 1;

			c = c >> 1;
		}

		crc_tab16[i] = crc;
	}

	crc_tab16_init = TRUE;

}

/*******************************************************************
unsigned short update_crc_16( unsigned short crc, char c )

PARÂMETROS:
			unsigned short old: Valor atual do CRC
			char byte: byte a ser utilizado na atualização do CRC

* DESCRIÇÃO : realiza o calculo do crc
*
* RETORNO :  crc calculado
*
********************************************************************/

unsigned short update_crc_16( unsigned short crc, char c ) {

	unsigned short tmp, short_c;

	short_c = 0x00ff & (unsigned short) c;

	if ( ! crc_tab16_init ) init_crc16_tab();

	tmp =  crc       ^ short_c;
	crc = (crc >> 8) ^ crc_tab16[ tmp & 0xff ];

	return crc;

}  /* update_crc_16 */



