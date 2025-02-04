#include <stdint.h>
#include "tm4c123gh6pm.h"

#define RED_LED (1U << 1)  // PF1 for the red LED

void PortF_Init(void);
void ADC0_Init(void);
uint16_t ADC0_Read(void);
char string(int x);
void print_UART(uint16_t x);

int main(void) {
    uint16_t adc_value;
    uint8_t high_state = 0;  // 0: Below 10mV, 1: Above 30mV

    SYSCTL_RCGCUART_R |= ( 1 << 0);

    SYSCTL_RCGC2_R |= (1 << 0);
    GPIO_PORTA_LOCK_R = 0x4C4F434B;
    GPIO_PORTA_CR_R = (1 << 1) | ( 1 << 0);
    GPIO_PORTA_DEN_R = (1 << 1) | ( 1 << 0);
    GPIO_PORTA_AFSEL_R = (1 << 1) | ( 1 << 0);
    GPIO_PORTA_PCTL_R = (1 << 0) | ( 1 << 4);




    UART0_CTL_R &= ~(1 << 0);
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 0x11;
    UART0_LCRH_R = (0x3 << 5) ;//|(1 << 1)|(1 << 7);
    UART0_CC_R = 0x0;

    UART0_CTL_R = (1 << 0)|(1 << 8)|(1 << 9);

    PortF_Init();  // Initialize LED
    ADC0_Init();   // Initialize ADC

    // Ensure LED is off by default
    GPIO_PORTF_DATA_R &= ~RED_LED;

    while (1) {
        adc_value = ADC0_Read()*0.8;  // Read ADC value

        print_UART(adc_value);
}
}
void PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= (1U << 5);  // Enable clock for Port F
    //while ((SYSCTL_PRGPIO_R & (1U << 5)) == 0);  // Wait for Port F to be ready

    GPIO_PORTF_DIR_R |= RED_LED;    // Set PF1 as output
    GPIO_PORTF_DEN_R |= RED_LED;    // Enable digital function for PF1

    GPIO_PORTF_DATA_R &= ~RED_LED;  // Ensure LED is off initially
}

void ADC0_Init(void) {
    SYSCTL_RCGCADC_R |= 1;  // Enable ADC0 clock
    SYSCTL_RCGCGPIO_R |= 0x10;  // Enable clock for Port E
    //while ((SYSCTL_PRGPIO_R & (1U << 4)) == 0);  // Wait for Port E to be ready

    GPIO_PORTE_AFSEL_R |= (1U << 3);  // Enable alternate function on PE3
    GPIO_PORTE_DEN_R &= ~(1U << 3);  // Disable digital I/O on PE3
    GPIO_PORTE_AMSEL_R |= (1U << 3);  // Enable analog function on PE3

    ADC0_ACTSS_R &= ~8;              // Disable sample sequencer 3
    ADC0_EMUX_R |= 0xF000;  // always sample for SS3
    ADC0_SSMUX3_R = 0;               // Set channel AIN0 (PE3)
    ADC0_SSCTL3_R = 6;               // End of sequence and enable interrupt
    ADC0_ACTSS_R |= 8;               // Enable sample sequencer 3
}

uint16_t ADC0_Read(void) {
    ADC0_PSSI_R = 8;                 // Start sampling on SS3
    while ((ADC0_RIS_R & 8) == 0);   // Wait for conversion to complete
    uint16_t result = ADC0_SSFIFO3_R & 0xFFF;  // Read 12-bit ADC value
    ADC0_ISC_R = 8;                  // Clear the completion flag
    return result;
}

void print_UART(uint16_t x)
{
        //int number= 12345678;
        int y= 1;
        int number_copy;
        int length=0;

        number_copy=x;

        while(number_copy>=y)
        {
            length++;
            y=y*10;
        }

        char char_num[20];

        int place_value=10;
        int rem;
        int index=0;
        while(number_copy>=place_value)
        {
            rem=number_copy%place_value;
            char_num[index]=string(rem);

            number_copy=number_copy-rem;
            number_copy=number_copy/10;
            index++;
        }

        char_num[length-1]=string(number_copy);
        //char_num[length+1]='/n';

        int a=length-1;
        while(a>=0)
        {
            UART0_DR_R = char_num[a];
            while (UART0_FR_R & 0x08)
            {
                ; // wait till transmission is complete
            }
            a--;
        }
        UART0_DR_R = '\n';
        while (UART0_FR_R & 0x08){
            ; // wait till transmission is complete
        }
        UART0_DR_R = '\r';
        while (UART0_FR_R & 0x08){
            ; // wait till transmission is complete
        }
}

char string(int x)
{
    char num;
    switch(x)
    {
    case 1:
        num= '1';
        break;
    case 2:
        num= '2';
        break;
    case 3:
        num= '3';
        break;
    case 4:
        num= '4';
        break;
    case 5:
        num= '5';
        break;
    case 6:
        num= '6';
        break;
    case 7:
        num= '7';
        break;
    case 8:
        num= '8';
        break;
    case 9:
        num= '9';
        break;
    case 0:
        num= '0';
        break;

    }
    return num;
}
