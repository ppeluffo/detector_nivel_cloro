/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

//#include <xc.h> // include processor files - each processor file is guarded.  
#include <avr/io.h>

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#ifndef F_CPU
#define F_CPU 24000000
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "croutine.h"
#include "semphr.h"
#include "timers.h"
#include "limits.h"
#include "portable.h"

#include "protected_io.h"
#include "ccp.h"

#include <avr/io.h>
#include <avr/builtins.h>
#include <avr/wdt.h> 
#include <avr/pgmspace.h>
        
#include "stdbool.h"
#include "frtos-io.h"
#include "xprintf.h"

#define FW_REV "1.0.0 beta"
#define FW_DATE "@ 20220518"
#define HW_MODELO "Sensor Nivel Cloro FRTOS R001 HW:AVR128DA64"
#define FRTOS_VERSION "FW:FreeRTOS V202111.00"

#define SYSMAINCLK 24

#define tkDac_TASK_PRIORITY	 	( tskIDLE_PRIORITY + 1 )
#define tkCtl_TASK_PRIORITY	 	( tskIDLE_PRIORITY + 1 )
#define tkSensor_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )
#define tkCmd_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkInputs_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )

#define tkDac_STACK_SIZE		384
#define tkCtl_STACK_SIZE		384
#define tkSensor_STACK_SIZE		384
#define tkCmd_STACK_SIZE		384
#define tkInputs_STACK_SIZE		384

StaticTask_t tkDac_Buffer_Ptr;
StackType_t tkDac_Buffer [tkDac_STACK_SIZE];

StaticTask_t tkCtl_Buffer_Ptr;
StackType_t tkCtl_Buffer [tkCtl_STACK_SIZE];

StaticTask_t tkSensor_Buffer_Ptr;
StackType_t tkSensor_Buffer [tkSensor_STACK_SIZE];

StaticTask_t tkCmd_Buffer_Ptr;
StackType_t tkCmd_Buffer [tkCmd_STACK_SIZE];

StaticTask_t tkInputs_Buffer_Ptr;
StackType_t tkInputs_Buffer [tkInputs_STACK_SIZE];

SemaphoreHandle_t sem_SYSVars;
StaticSemaphore_t SYSVARS_xMutexBuffer;
#define MSTOTAKESYSVARSSEMPH ((  TickType_t ) 10 )

TaskHandle_t xHandle_tkDac, xHandle_tkCtl, xHandle_tkSensor, xHandle_tkCmd, xHandle_tkInputs;

void tkDac(void * pvParameters);
void tkCtl(void * pvParameters);
void tkSensor(void * pvParameters);
void tkCmd(void * pvParameters);
void tkInputs(void * pvParameters);

void system_init();
void reset(void);

int xprintf( const char *fmt, ...);
void xputChar(unsigned char c);

void kick_wdt( uint8_t bit_pos);

void acgen_run(void);
void acgen_stop(void);
void acgen_freq_hz(uint16_t freq);

void config_default(void);
bool save_config_in_NVM(void);
bool load_config_from_NVM(void);


void acgen_setup_outofrtos(void );

#define MAX_LENGTH 32

bool starting_flag;

struct {
    uint16_t dac;
    bool acgen_running;
    uint16_t acgen_freq_hz;
    uint8_t checksum;
    uint16_t dinputs;

} systemVars;

uint8_t sys_watchdog;

#define CMD_WDG_bm 0x01
#define CMD_WDG_bp    0
#define CTL_WDG_bm 0x02
#define CTL_WDG_bp    1
#define DAC_WDG_bm 0x04
#define DAC_WDG_bp    2
//#define LRA_WDG_bm 0x08
//#define LRA_WDG_bp    3

#define WDG_bm 0x07

#define WDG_INIT() ( sys_watchdog = WDG_bm )


#endif	/* XC_HEADER_TEMPLATE_H */

