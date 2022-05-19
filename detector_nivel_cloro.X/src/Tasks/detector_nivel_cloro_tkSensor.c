/*
 * File:   tk03.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "detector_nivel_cloro.h"
#include "pines.h"
#include "led.h"

StaticTimer_t acgen_xTimerBuffer;
TimerHandle_t acgen_xTimer;
void acgen_TimerCallback( TimerHandle_t xTimer );

//------------------------------------------------------------------------------
void tkSensor(void * pvParameters)
{

    /*
     * Esta funcion se encarga de generar el voltaje AC y de medir las
     * entradas de los electrodos.
     * En lo que respecta a la generacion de AC, tenemos una variable de
     * control ( systemVars ) , acgen_running, que indica si activamos o no
     * la generacion de AC
     * 
     */

( void ) pvParameters;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    xprintf("Starting tkSensor..\r\n");
         
    acgen_run();

	for( ;; )
	{
                
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        
	}
}
//------------------------------------------------------------------------------
void acgen_setup_outofrtos(void )
{
	// Configuro el timer que va a generar los pulsos del stepper
	// Se debe correr antes que empieze el RTOS

	acgen_xTimer = xTimerCreateStatic (
            "ACGEN",
			pdMS_TO_TICKS( 10 ),
			pdTRUE,
			( void * ) 0,
			acgen_TimerCallback,
			&acgen_xTimerBuffer
			);

}
//------------------------------------------------------------------------------
void acgen_TimerCallback( TimerHandle_t xTimer )
{
    TOGGLE_AC0();
    TOGGLE_AC1();
}
//------------------------------------------------------------------------------
void acgen_run(void)
{
  
uint16_t timer_period_ms;

    timer_period_ms = 1000 / systemVars.acgen_freq_hz;
    
    xTimerChangePeriod( acgen_xTimer, pdMS_TO_TICKS( timer_period_ms ), 10 );
                                                           
        
    while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
  		vTaskDelay( ( TickType_t)( 1 / portTICK_PERIOD_MS ) );
    
    
    systemVars.acgen_running = true;
    
    xSemaphoreGive( sem_SYSVars );
    
    ACGEN_set_pin(ACPIN0,0 );
    ACGEN_set_pin(ACPIN1,1 );
    
    
    // Arrancamos el timer
    if ( xTimerStart( acgen_xTimer, 0 )  != pdPASS ) {
        xprintf("ALERT: Timer not started. !!\r\n");
    }
}
//------------------------------------------------------------------------------
void acgen_stop(void)
{
    // Detengo el timer
    if ( xTimerStop( acgen_xTimer, 0 )  != pdPASS ) {
        xprintf("ALERT: Timer not stopped. !!\r\n");
    }
    
    while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
  		vTaskDelay( ( TickType_t)( 1 / portTICK_PERIOD_MS ) );
    
    systemVars.acgen_running = false;
    
    xSemaphoreGive( sem_SYSVars );
    
    ACGEN_set_pin(ACPIN0,0 );
    ACGEN_set_pin(ACPIN1,0 );
}
//------------------------------------------------------------------------------
void acgen_freq_hz(uint16_t freq)
{
    systemVars.acgen_freq_hz = freq;
}
//------------------------------------------------------------------------------
