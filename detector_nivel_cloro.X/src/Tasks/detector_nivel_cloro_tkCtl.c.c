/*
 * File:   tkCtl.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "detector_nivel_cloro.h"
#include "led.h"
#include "usart.h"

#define TKCTL_DELAY_S	1

void sys_watchdog_check(void);

//------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
    
	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    xprintf_P(PSTR("Starting tkCtl..\r\n"));
    
    //if ( ! load_config_from_NVM()) 
    //    config_default();
    
    WDG_INIT();

    // Siempre que arranco voy al modo NORMAL
    systemVars.modo = MODO_NORMAL;
    systemVars.debug = false;
    
    // Habilito a todas las otras tareas a arrancar
    starting_flag = true;
       
	for( ;; )
	{
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        kick_wdt(CTL_WDG_bp);
        led_flash();
        sys_watchdog_check();
	}
}
//------------------------------------------------------------------------------
void sys_watchdog_check(void)
{
    // El watchdog se inicializa en 7.
    // Cada tarea debe poner su bit en 0. Si alguna no puede, se resetea
    
    //wdt_reset();
    //return;
    
    if ( sys_watchdog != 0 ) {  
        xprintf_P(PSTR("tkCtl: reset by wdg [0x%02d]\r\n"), sys_watchdog );
        vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        reset();
    } else {
        wdt_reset();
        WDG_INIT();
    }
}
//------------------------------------------------------------------------------