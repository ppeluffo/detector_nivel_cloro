#include "detector_nivel_cloro.h"

#include "frtos_cmd.h"
#include "pines.h"
#include "dac.h"
#include "adc.h"


static void cmdClsFunction(void);
static void cmdHelpFunction(void);
static void cmdResetFunction(void);
static void cmdStatusFunction(void);
static void cmdWriteFunction(void);
static void cmdReadFunction(void);
static void cmdConfigFunction(void);
static void cmdTestFunction(void);

static void pv_snprintfP_OK(void );
static void pv_snprintfP_ERR(void );

//------------------------------------------------------------------------------
void tkCmd(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );

uint8_t c = 0;

    FRTOS_CMD_init();

    FRTOS_CMD_register( "cls", cmdClsFunction );
	FRTOS_CMD_register( "help", cmdHelpFunction );
    FRTOS_CMD_register( "reset", cmdResetFunction );
    FRTOS_CMD_register( "status", cmdStatusFunction );
    FRTOS_CMD_register( "write", cmdWriteFunction );
    FRTOS_CMD_register( "read", cmdReadFunction );
    FRTOS_CMD_register( "config", cmdConfigFunction );
    FRTOS_CMD_register( "test", cmdTestFunction );
    
    xprintf_P(PSTR("Starting tkCmd..\r\n" ));
    xprintf_P(PSTR("Spymovil %s %s %s %s \r\n") , HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);
    
	// loop
	for( ;; )
	{
        kick_wdt(CMD_WDG_bp);
         
		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdTERM, (char *)&c, 1 ) == 1 ) {
            FRTOS_CMD_process(c);
        }
        //vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	}    
}
//------------------------------------------------------------------------------
static void cmdTestFunction(void)
{

    FRTOS_CMD_makeArgv();
        
int8_t i;
char c;
float f;
char s1[20];

	xprintf("Test function\r\n");
    i=10;
    xprintf("Integer: %d\r\n", i);
    c='P';
    xprintf("Char: %c\r\n", c);
    f=12.32;
    xprintf("FLoat: %0.3f\r\n", f);
    
    strncpy(s1,"Pablo Peluffo", 20);
    xprintf("String: %s\r\n", s1);
    // 
    xprintf("Todo junto: [d]=%d, [c]=%c, [s]=%s, [f]=%0.3f\r\n",i,c,s1,f);
    
    // STRINGS IN ROM
    
    xprintf_P(PSTR("Strings in ROM\r\n"));
    i=11;
    xprintf_P(PSTR("Integer: %d\r\n"), i);
    c='Q';
    xprintf_P(PSTR("Char: %c\r\n"), c);

    f=15.563;
    xprintf_P(PSTR("FLoat: %0.3f\r\n"), f);
    strncpy(s1,"Keynet Spymovil", 20);
    xprintf_P(PSTR("String: %s\r\n"), s1);
    // 
    xprintf_P(PSTR("Todo junto: [d]=%d, [c]=%c, [s]=%s, [f]=%0.3f\r\n"),i,c,s1,f);
   
    // DEFINED
    xprintf("Spymovil %s %s %s %s \r\n" , HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);
    
}
//------------------------------------------------------------------------------
static void cmdHelpFunction(void)
{

    FRTOS_CMD_makeArgv();
        
    if ( strcmp( strupr(argv[1]), "WRITE") == 0 ) {
		xprintf("-write:\r\n");
        xprintf("   dac{val}\r\n");
        
    }  else if ( strcmp( strupr(argv[1]), "CONFIG") == 0 ) {
		xprintf("-config:\r\n");
		xprintf("   default,save,load\r\n");
        xprintf("   modo {NORMAL,DIAG}\r\n");
        xprintf("   debug {ON,OFF}\r\n");
        xprintf("   VREF_ADC,VREF_ETAPE\r\n");
        xprintf("   RETAPE_FIX,RETAPE_HMIN,RETAPE_HMAX\r\n");
        xprintf("   HETAPE_MIN,HETAPE_MAX\r\n");

    }  else if ( strcmp( strupr(argv[1]), "READ") == 0 ) {
		xprintf("-read:\r\n");
		xprintf("   adc {samples}\r\n");
        
    }  else {
        // HELP GENERAL
        xprintf("Available commands are:\r\n");
        xprintf("-cls\r\n");
        xprintf("-help\r\n");
        xprintf("-status\r\n");
        xprintf("-reset\r\n");
        xprintf("-write...\r\n");
        xprintf("-config...\r\n");
        xprintf("-read...\r\n");

    }
   
	xprintf("Exit help \r\n");

}
//------------------------------------------------------------------------------
static void cmdClsFunction(void)
{
	// ESC [ 2 J
	xprintf("\x1B[2J\0");
}
//------------------------------------------------------------------------------
static void cmdResetFunction(void)
{
    xprintf("Reset..\r\n");
    reset();
}
//------------------------------------------------------------------------------
static void cmdStatusFunction(void)
{

    // https://stackoverflow.com/questions/12844117/printing-defined-constants
    
    xprintf("Spymovil %s %s %s %s \r\n" , HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);
    // modo
    if ( systemVars.modo == MODO_NORMAL ) {
        xprintf_P(PSTR(" Modo: NORMAL\r\n"));
    } else {
        xprintf_P(PSTR(" Modo: DIAGNOSTICO\r\n"));
    }
    // debug
    if ( systemVars.debug ) {
        xprintf_P(PSTR(" Debug: ON\r\n"));
    } else {
        xprintf_P(PSTR(" Debug: OFF\r\n"));
    }
    xprintf_P(PSTR(" VREF_ADC=%0.3f\r\n"), systemConf.VREF_ADC);
    xprintf_P(PSTR(" VREF_ETAPE=%0.3f\r\n"), systemConf.VREF_ETAPE);
    xprintf_P(PSTR(" RETAPE_FIX=%d\r\n"), systemConf.RETAPE_FIX);
    xprintf_P(PSTR(" RETAPE_HMIN=%d\r\n"), systemConf.RETAPE_HMIN);
    xprintf_P(PSTR(" RETAPE_HMAX=%d\r\n"), systemConf.RETAPE_HMAX);
    xprintf_P(PSTR(" HETAPE_MIN=%d\r\n"), systemConf.HETAPE_MIN);
    xprintf_P(PSTR(" HETAPE_MAX=%d\r\n"), systemConf.HETAPE_MAX);
    
    xprintf_P(PSTR(" DAC=%d\r\n"),systemVars.dac);
    xprintf_P(PSTR(" ADC=%d\r\n"),systemVars.adc);
    
}
//------------------------------------------------------------------------------
static void cmdWriteFunction(void)
{

    FRTOS_CMD_makeArgv(); 
        
    // write DAC
    if ( strcmp( strupr(argv[1]),"DAC") == 0 ) {
        systemVars.dac = atoi(argv[2]);
        DAC_setVal(systemVars.dac);
        pv_snprintfP_OK();
        return;
    }

    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void cmdReadFunction(void)
{
 
    
uint8_t samples;

    FRTOS_CMD_makeArgv();
        
    // read ADC
    if ( strcmp( strupr(argv[1]),"ADC") == 0 ) {
        samples = atoi(argv[2]);
        if ( ( samples == 0) || (samples > 32)) {
            pv_snprintfP_ERR();
            return;
        }

        systemVars.adc = ADC_read(samples);
        
        xprintf_P(PSTR("ADC=%d\r\n"), systemVars.adc);
        pv_snprintfP_OK();
        return;
    }
    
    
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void cmdConfigFunction(void)
{
  
    FRTOS_CMD_makeArgv();
       
    // default
	if ( strcmp( strupr(argv[1]),"DEFAULT") == 0  ) {
		config_default();
		return;
	}

    // HETAPE_MAX
    if ( strcmp( strupr(argv[1]),"HETAPE_MAX") == 0  ) {
        systemConf.HETAPE_MAX = atoi(argv[2]);
        pv_snprintfP_OK();
        return;   
    }
    
    // HETAPE_MIN
    if ( strcmp( strupr(argv[1]),"HETAPE_MIN") == 0  ) {
        systemConf.HETAPE_MIN = atoi(argv[2]);
        pv_snprintfP_OK();
        return;   
    }
    
    // RETAPE_HMAX
    if ( strcmp( strupr(argv[1]),"RETAPE_HMAX") == 0  ) {
        systemConf.RETAPE_HMAX = atoi(argv[2]);
        pv_snprintfP_OK();
        return;   
    }

    // RETAPE_HMIN
    if ( strcmp( strupr(argv[1]),"RETAPE_HMIN") == 0  ) {
        systemConf.RETAPE_HMIN = atoi(argv[2]);
        pv_snprintfP_OK();
        return;   
    }
    
    // RETAPE_FIX
    if ( strcmp( strupr(argv[1]),"RETAPE_FIX") == 0  ) {
        systemConf.RETAPE_FIX = atoi(argv[2]);
        pv_snprintfP_OK();
        return;   
    }
    
    // VREF_ETAPE
    if ( strcmp( strupr(argv[1]),"VREF_ETAPE") == 0  ) {
        systemConf.VREF_ETAPE = atof(argv[2]);
        pv_snprintfP_OK();
        return;   
    }
    
    // VREF_ADC
    if ( strcmp( strupr(argv[1]),"VREF_ADC") == 0  ) {
        systemConf.VREF_ADC = atof(argv[2]);
        pv_snprintfP_OK();
        return;   
    }
    
    // modo (NORMAL,DIAG)
	if ( strcmp( strupr(argv[1]),"MODO") == 0  ) {
		if ( strcmp( strupr(argv[2]),"NORMAL") == 0  ) {
            systemVars.modo = MODO_NORMAL;
            pv_snprintfP_OK();
            return;
        }
		if ( strcmp( strupr(argv[2]),"DIAG") == 0  ) {
            systemVars.modo = MODO_DIAGNOSTICO;
             pv_snprintfP_OK();
             return;
        }        
        pv_snprintfP_ERR();
        return;        
	}
    
    // debug (ON,OFF)
	if ( strcmp( strupr(argv[1]),"DEBUG") == 0  ) {
		if ( strcmp( strupr(argv[2]),"ON") == 0  ) {
            systemVars.debug = true;
             pv_snprintfP_OK();
             return;
        }
		if ( strcmp( strupr(argv[2]),"OFF") == 0  ) {
            systemVars.debug = false;
             pv_snprintfP_OK();
             return;
        }        
        pv_snprintfP_ERR();
        return;        
	}
    
    // save
    if ( strcmp( strupr(argv[1]),"SAVE") == 0 ) {
        save_config_in_NVM() ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
    }
    
    // load
    if ( strcmp( strupr(argv[1]),"LOAD") == 0 ) {
        load_config_from_NVM() ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
    }
    
        
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void pv_snprintfP_OK(void )
{
	xprintf("ok\r\n\0");
}
//------------------------------------------------------------------------------
static void pv_snprintfP_ERR(void)
{
	xprintf("error\r\n\0");
}
//------------------------------------------------------------------------------

