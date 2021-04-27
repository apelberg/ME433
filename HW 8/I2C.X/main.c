#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "math.h"
//#include "i2c_master_noint.c"
#include "i2c_master_noint.h"

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

void setPin(unsigned char address, unsigned char reg, unsigned char value);
unsigned char readPin(unsigned char waddress, unsigned char reg,unsigned char raddress);
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
    
    i2c_master_setup();
    
    setPin(0b01000000, 0x00, 0x00); // set IODIRA output
    setPin(0b01000000, 0x01, 0xFF); // set IODIRB input

    
    __builtin_enable_interrupts();
    while(1){
        //LATAINV = 0b10000; // Switch A4
        LATAINV = 0b10000;
        delay(0.05);
        
        if (readPin(0b01000000, 0x19, 0b01000001) == 0){
            setPin(0b01000000,0x0A,0b10000000); // setting GPA7 high
        }
        else{
            setPin(0b01000000,0x0A,0b00000000);
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

void setPin(unsigned char address, unsigned char reg, unsigned char value){
     i2c_master_start(); // send start bit
     i2c_master_send(address); // send write address
     i2c_master_send(reg); // send register
     i2c_master_send(value); // send value
     i2c_master_stop(); // send stop bit
}

unsigned char readPin(unsigned char waddress, unsigned char reg,unsigned char raddress){
    i2c_master_start(); // send start bit
    i2c_master_send(waddress); // send write address
    i2c_master_send(reg); // send register
    i2c_master_restart();
    i2c_master_send(raddress); // send write address
    unsigned char value = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop(); // send stop bit
    return value;
    
}