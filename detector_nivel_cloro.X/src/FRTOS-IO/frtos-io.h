/*
 * frtos-io.h
 *
 *  Created on: 11 jul. 2018
 *      Author: pablo
 *
 * Funcionamiento:
 * Usamos los servicios que nos brindan los drivers.
 * Definimos para cada periferico una estructura de control que depende del periferico.
 * En el caso del los puertos seriales es periferico_serial_port_t.
 * Este tiene un elemento que es un puntero a una uart definida en el driver.
 *
 * Cada periferico se asocia a un file descriptor de modo que las funciones genericas
 * frtos_open/ioctl/read/write por medio de un switch redirigen a funciones mas especializadas
 * en cada tipo de periferico.
 *
 */

#ifndef SRC_FRTOS_IO_FRTOS_IO_H_
#define SRC_FRTOS_IO_FRTOS_IO_H_


#include <avr/io.h>
#include <avr/interrupt.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "drv_uart_avrDX.h"
#include "drv_nvm_avrDX.h"

// Identificador de los file descriptor.
typedef enum {
	fdTERM = 0,  
    fdMODBUS,
    fdNVM,
            
} file_descriptor_t;


typedef struct {
	file_descriptor_t fd;
	SemaphoreHandle_t xBusSemaphore;		//
	uint8_t xBlockTime;						// ticks to block in read operations. Set by ioctl
    uint16_t eeAddress;
} periferico_nvm_t;

// Perifericos real.
periferico_nvm_t xNVM;

StaticSemaphore_t NVM_xMutexBuffer;

#define ioctl_OBTAIN_BUS_SEMPH			1
#define ioctl_RELEASE_BUS_SEMPH			2
#define ioctl_SET_TIMEOUT				3

#define ioctl_UART_CLEAR_RX_BUFFER		4
#define ioctl_UART_CLEAR_TX_BUFFER		5
#define ioctl_UART_ENABLE_TX_INT		6
#define ioctl_UART_DISABLE_TX_INT		7
#define ioctl_UART_ENABLE_RX_INT		8
#define ioctl_UART_DISABLE_RX_INT		9
#define ioctl_UART_ENABLE_TX			10
#define ioctl_UART_DISABLE_TX			11
#define ioctl_UART_ENABLE_RX			12
#define ioctl_UART_DISABLE_RX			13

#define ioctl_NVM_SET_EEADDRESS         30

int16_t frtos_open( file_descriptor_t fd, uint32_t flags);
void frtos_open_uart0(uint32_t baudrate);
void frtos_open_uart1(uint32_t baudrate);
int16_t frtos_nvm_open( periferico_nvm_t *xNVM, file_descriptor_t fd, StaticSemaphore_t *i2c_semph, uint32_t flags);

int16_t frtos_write( file_descriptor_t fd ,const char *pvBuffer, const uint16_t xBytes );
int16_t frtos_uart0_write( const char *pvBuffer, const uint16_t xBytes );
int16_t frtos_uart1_write( const char *pvBuffer, const uint16_t xBytes );
int16_t frtos_nvm_write( periferico_nvm_t *xNVM, const char *pvBuffer, const uint16_t xBytes );

int16_t frtos_ioctl( file_descriptor_t fd, uint32_t ulRequest, void *pvValue );
int16_t frtos_ioctl_uart0( uint32_t ulRequest, void *pvValue );
int16_t frtos_ioctl_uart1( uint32_t ulRequest, void *pvValue );
int16_t frtos_nvm_ioctl( periferico_nvm_t *xNVM, uint32_t ulRequest, void *pvValue );

int16_t frtos_read( file_descriptor_t fd , char *pvBuffer, uint16_t xBytes );
int16_t frtos_read_uart0( char *pvBuffer, uint16_t xBytes );
int16_t frtos_read_uart1( char *pvBuffer, uint16_t xBytes );
int16_t frtos_nvm_read( periferico_nvm_t *xNVM, char *pvBuffer, const uint16_t xBytes );


#endif /* SRC_FRTOS_IO_FRTOS_IO_H_ */
