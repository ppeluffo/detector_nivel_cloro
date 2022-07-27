/* 
 * File:   frtos20_utils.c
 * Author: pablo
 *
 * Created on 22 de diciembre de 2021, 07:34 AM
 */



#include "detector_nivel_cloro.h"
#include "xprintf.h"
#include "led.h"
#include "dac.h"
#include "pines.h"
#include "nvm.h"
#include "adc.h"

//------------------------------------------------------------------------------
int8_t WDT_init(void);
int8_t CLKCTRL_init(void);
uint8_t checksum( uint8_t *s, uint16_t size );

//-----------------------------------------------------------------------------
void system_init()
{

	CLKCTRL_init();
    WDT_init();
    LED_init();
    XPRINTF_init();
    DAC_init();
    ADC_init();
    
}
//-----------------------------------------------------------------------------
int8_t WDT_init(void)
{
	/* 8K cycles (8.2s) */
	/* Off */
	ccp_write_io((void *)&(WDT.CTRLA), WDT_PERIOD_8KCLK_gc | WDT_WINDOW_OFF_gc );  
	return 0;
}
//-----------------------------------------------------------------------------
int8_t CLKCTRL_init(void)
{
	// Configuro el clock para 24Mhz
	
	ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), CLKCTRL_FREQSEL_24M_gc         /* 24 */
	| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
	| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKCTRLA),CLKCTRL_CLKSEL_OSCHF_gc /* Internal high-frequency oscillator */
	//		 | 0 << CLKCTRL_CLKOUT_bp /* System clock out: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKLOCK),0 << CLKCTRL_LOCKEN_bp /* lock enable: disabled */);

	return 0;
}
//-----------------------------------------------------------------------------
void reset(void)
{
	/* Issue a Software Reset to initilize the CPU */
	ccp_write_io( (void *)&(RSTCTRL.SWRR), RSTCTRL_SWRST_bm ); 
                                           
}
//------------------------------------------------------------------------------
void kick_wdt( uint8_t bit_pos)
{
    // Pone el bit correspondiente en 0.
    sys_watchdog &= ~ (1 << bit_pos);
    
}
//------------------------------------------------------------------------------
void config_default(void)
{

   systemConf.VREF_ADC = 2.5;
   systemConf.VREF_ETAPE = 3.3;
   systemConf.RETAPE_FIX = 2400;
   systemConf.HETAPE_MIN = 3;
   systemConf.HETAPE_MAX = 45.0;
   systemConf.RETAPE_HMIN = 400;
   systemConf.RETAPE_HMAX = 2400;
}
//------------------------------------------------------------------------------
bool save_config_in_NVM(void)
{
   
int8_t retVal;
uint8_t cks;

    cks = checksum ( (uint8_t *)&systemConf, ( sizeof(systemConf) - 1));
    systemConf.checksum = cks;
    
    retVal = NVM_EE_write( 0x00, (char *)&systemConf, sizeof(systemConf) );
    if (retVal == -1 )
        return(false);
    
    return(true);
   
}
//------------------------------------------------------------------------------
bool load_config_from_NVM(void)
{

uint8_t rd_cks, calc_cks;
    
    NVM_EE_read( 0x00, (char *)&systemConf, sizeof(systemConf) );
    rd_cks = systemConf.checksum;
    
    calc_cks = checksum ( (uint8_t *)&systemConf, ( sizeof(systemConf) - 1));
    
    if ( calc_cks != rd_cks ) {
		xprintf_P( PSTR("ERROR: Checksum systemVars failed: calc[0x%0x], read[0x%0x]\r\n"), calc_cks, rd_cks );
        
		return(false);
	}
    
    return(true);
}
//------------------------------------------------------------------------------
uint8_t checksum( uint8_t *s, uint16_t size )
{
	/*
	 * Recibe un puntero a una estructura y un tamaño.
	 * Recorre la estructura en forma lineal y calcula el checksum
	 */

uint8_t *p = NULL;
uint8_t cks = 0;
uint16_t i = 0;

	cks = 0;
	p = s;
	for ( i = 0; i < size ; i++) {
		 cks = (cks + (int)(p[i])) % 256;
	}

	return(cks);
}
//------------------------------------------------------------------------------
void convert_adc2vin(void)
{
    /*
     Convierte el valor del ADC al votaje que esta a la entrada
    */
    systemVars.vin_adc = systemVars.adc * systemConf.VREF_ADC / 4096;
    if (systemVars.debug) {
        xprintf_P(PSTR("DEBUG: vinadc=%0.3f\r\n"), systemVars.vin_adc);
    }
}
//------------------------------------------------------------------------------
void convert_adcvin2retape(void)
{
    /*
     Convierte el votaje que esta a la entrada del adc en la rEtape
    */
    systemVars.r_etape = systemVars.vin_adc * systemConf.RETAPE_FIX / ( systemConf.VREF_ETAPE - systemVars.vin_adc );
    if (systemVars.debug) {
        xprintf_P(PSTR("DEBUG: rEtape=%0.3f\r\n"), systemVars.r_etape);
    }
}
//------------------------------------------------------------------------------
void convert_retape2hetape(void)
{
    /*
     Convierte la resistencia rEtape en la altura.
    */
    systemVars.h_etape = ( systemVars.r_etape - systemConf.RETAPE_HMIN ) * ( systemConf.HETAPE_MAX - systemConf.HETAPE_MIN);
    systemVars.h_etape /= (systemConf.RETAPE_HMAX - systemConf.RETAPE_HMIN);
    systemVars.h_etape += systemConf.HETAPE_MIN;
    if (systemVars.debug) {
        xprintf_P(PSTR("DEBUG: hEtape=%d\r\n"), systemVars.h_etape );
    }
    
}
//------------------------------------------------------------------------------
void convert_hetape2dac(void)
{
    /*
     Convierte la altura de la eTape en el valor de DAC
    */
    
float dac;
    
    dac = (1024-204)/(systemConf.HETAPE_MAX - systemConf.HETAPE_MIN);
    dac *= ( systemVars.h_etape - systemConf.HETAPE_MIN);
    dac += 204;
    systemVars.dac = (uint16_t) dac;
    
    
    if (systemVars.debug) {
        xprintf_P(PSTR("DEBUG: dac=%d\r\n"), systemVars.dac );
    }
    
}
//------------------------------------------------------------------------------
void convert_adc2dac(void)
{
    /*
     * Convierto el valor del ADC ( etape ) el DAC ( corriente )
     * Mido el ADC un voltaje que es el del divisor resitivo de la ETAPE
     * Este voltaje es proporcional a la resistencia Etape  
    */
    
  // Altura del la eTape

float icc;

    // A partir del valor del ADC calculo la altura de la eTape
    convert_adc2vin();
    convert_adcvin2retape();
    convert_retape2hetape();
    convert_hetape2dac();


    // Convierto la altura a 4-20mA
    icc = 20 * systemVars.dac / 1024;
    
    if (systemVars.debug) {
        xprintf_P(PSTR("DEBUG: icc=%0.3f\r\n"), icc);
        
    }    
}
//------------------------------------------------------------------------------