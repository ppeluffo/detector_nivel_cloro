

#include "detector_nivel_cloro.h"
#include "pines.h"

//------------------------------------------------------------------------------
void tkInputs(void * pvParameters)
{

    /*
     * Leo las entradas digitales.
     */

( void ) pvParameters;
uint8_t i;
uint8_t pin;

uint16_t dinputs;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    xprintf("Starting tkInputs..\r\n");
         
	for( ;; )
	{
                
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        
        dinputs = 0x00;
        for (i=0;i<9; i++) {
            pin = pinputs_read(i);
            dinputs |= (pin<<i);
        }
        
        systemVars.dinputs = dinputs;
	}
}
//------------------------------------------------------------------------------

