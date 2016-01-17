/*
 * crc.h
 *
 *  Created on: 22/12/2015
 *      Author: Leandro
 */

#ifndef INCLUDE_CRC_H_
#define INCLUDE_CRC_H_


 /*****************************************************************************
* � 2015 Time Energy Ltda. Todos os direitos reservados.
*
* ATEN��O: Este arquivo possui informa��es confidenciais e n�o pode ser
* reproduzido ou distribu�do(total ou parcial) , sem a previa autoriza��o
* por escrito da Time Energy.
*****************************************************************************/

/***************************************************************************
 * @file 	lib_crc.h
 * @brief	Biblioteca para calcular do crc
 * @author	Renan
 * @date	23/09/2014
 * @company	Time Energy
 *
 * {Arquivos de Include}
****************************************************************************/
#include <c_types.h>
/*****************************************************************************
* COSNTANTES
*****************************************************************************/

/*****************************************************************************
* TIPOS
*****************************************************************************/

/*****************************************************************************
* VARI�VEIS GLOBAIS
*****************************************************************************/

/*****************************************************************************
* FUN��ES P�BLICAS
*****************************************************************************/


unsigned short          update_crc_16(     unsigned short crc, char c                 );
unsigned short update_crc_ansi(unsigned short old, char byte, char CONTROL);




#endif /* INCLUDE_CRC_H_ */
