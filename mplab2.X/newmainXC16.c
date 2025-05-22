
#pragma config POSCMOD = XT             
 
#pragma config OSCIOFNC = ON           
 
#pragma config FCKSM = CSDCMD           
 
#pragma config FNOSC = PRI              
 
#pragma config IESO = ON                
 
#pragma config WDTPS = PS32768        
 
#pragma config FWPSA = PR128            
 
#pragma config WINDIS = ON              
 
#pragma config FWDTEN = ON              
 
#pragma config ICS = PGx2               
 
#pragma config GWRP = OFF               
 
#pragma config GCP = OFF                
 
#pragma config JTAGEN = OFF    
 

#include "xc.h"
#include "libpic30.h"
#include "adc.h"
#include "buttons.h"
#include <stdlib.h>
 
 


void configure_system(void) {
    TRISA = 0x0000;
    TRISB |= (1 << 3) | (1 << 4);
    AD1PCFG = 0xFFFB;
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
}

int main(void) {
    uint8_t output_state = 0x00;
    uint8_t bit_shift_forward = 0x01;
    uint8_t bit_shift_backward = 0x80;
    int is_alarm_triggered = 0;
    int is_blinking_phase = 0;
    unsigned long blink_toggle_counter = 0;
    unsigned long blink_timer = 0;
    char program_mode = 7;

    char button6_current = 0, button6_previous = 0;
    char button13_current = 0, button13_previous = 0;

    configure_system();

    output_state = 0x00;

    while (1) {
        unsigned int pot_value = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);

        LATA = output_state;

        button6_previous = PORTDbits.RD6;
        button13_previous = PORTDbits.RD13;
        __delay32(50000);
        button6_current = PORTDbits.RD6;
        button13_current = PORTDbits.RD13;

        if (!is_alarm_triggered && pot_value < 512) {
            is_alarm_triggered = 1;
            is_blinking_phase = 1;
            blink_toggle_counter = 0;
            blink_timer = 0;
        }

        if (is_alarm_triggered) {
            if (pot_value >= 512 || button13_current == 0) {
                is_alarm_triggered = 0;
                LATA = 0x00;
                continue;
            }

            if (is_blinking_phase) {
                if (blink_toggle_counter % 2) {
                    LATA = 0x01;
                } else {
                    LATA = 0x00;
                }

                __delay32(1000000);
                blink_toggle_counter++;
                blink_timer += 250000;

                if (blink_timer >= 5000000) {
                    is_blinking_phase = 0;
                }
            } else {
                __delay32(1000000);
                LATA = 0xFF;
                __delay32(1000000);
            }
        } else {
            LATA = 0x00;
        }
    }
    return 0;
}
