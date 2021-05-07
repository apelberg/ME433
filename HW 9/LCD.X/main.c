#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ST7789.h"
#include"font.h"
#include<stdio.h>
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
    __builtin_enable_interrupts();
    
    initSPI();
    LCD_init();
    
    // initialize heartbeat pin
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    
    LCD_clearScreen(WHITE);
    char m [50];
    int num;
    
    while(1){
        num = 0;
        int i;
        for (i = 0; i<101; i++){
            sprintf(m, "Hello World %d!  ", num);
            _CP0_SET_COUNT(0);
            drawString(28,32,m,BLUE, WHITE);
            int ticks = _CP0_GET_COUNT();
            
            drawProgressBar(28, 44, 101, i, BLUE, RED);
            
            float frequency = 1/(ticks /24000000.0);
            sprintf(m, "FPS: %.3f", frequency);
            drawString(28,60,m,BLUE, WHITE);
            
            // heartbeat
            LATAINV = 0b10000;
            delay(0.01);
            num++;
        }
   }
}

void delay(float time){
    float clocks = time/(1.0/24000000.0);
    _CP0_SET_COUNT(0); // reset count
        while (_CP0_GET_COUNT() < clocks){ 
            ;
        }
    
}