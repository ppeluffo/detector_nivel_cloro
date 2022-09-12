/*
 * File:   tkSystem.c
 * Author: pablo
 *
 */

#include "detector_nivel_cloro.h"
#include "adc.h"
#include "dac.h"


#define TKSYSTEM_DELAY_MS   30000

//------------------------------------------------------------------------------
void tkSystem(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
    
    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

	//vTaskDelay( ( TickType_t)( 400 / portTICK_PERIOD_MS ) );
    
    xprintf_P(PSTR("Starting tkSystem..\r\n"));
    
	for( ;; )
	{
		vTaskDelay( ( TickType_t)( systemConf.timerpoll * 1000 / portTICK_PERIOD_MS ) );
        kick_wdt(SYS_WDG_bp);
        
        switch(systemVars.modo) {
            case MODO_NORMAL:
                // Poleo la entrada analogica y la pongo en la salida
                poll_sensor();
                break;
            case MODO_DIAGNOSTICO:
                // No hago nada. Solo acepto comandos en tkCMD
                break;
        }
     
	}
}
//------------------------------------------------------------------------------

