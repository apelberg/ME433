#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ws2812b.h"

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

void delay(float time);

int main(void){
    __builtin_disable_interrupts();
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    
    ws2812b_setup();
    
    
    __builtin_enable_interrupts();
    float i = 0;
    float j = 90;
    float k = 180;
    float l = 270;
    while(1){
        
            int numLEDs = 4;
            wsColor c[numLEDs];
            c[0] = HSBtoRGB(i, 1, 1);
            c[1] = HSBtoRGB(j, 1, 1);
            c[2] = HSBtoRGB(k, 1, 1);
            c[3] = HSBtoRGB(l, 1, 1);
            i = i+1;
            j = j+1;
            k = k+1;
            l= l+1;
                    
            if (i - 360 >  0){
                i = 0;
            }
            if (j - 360 >  0){
                j = 0;
            }
            if (k - 360 >  0){
                k = 0;
            }
            if (l - 360 >  0){
                l = 0;
            }
            // heartbeat
            LATAINV = 0b10000;
            delay(0.001);

            ws2812b_setColor(c, numLEDs);
        }
       
   }


void delay(float time){
    float clocks = time/(1.0/24000000.0);
    _CP0_SET_COUNT(0); // reset count
        while (_CP0_GET_COUNT() < clocks){ 
            ;
        }
    
}