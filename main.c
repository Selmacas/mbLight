#include <msp430g2553.h>
#include <stdint.h>
#include "modbus_lib.h"

#define MB_ADR 123
#define LED_STEPS 101

#if LED_STEPS == 101
const uint16_t lut[] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 7, 8, 8, 9, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35, 38, 41, 44, 47, 50, 54, 58, 62, 67, 71, 77, 82, 88, 94, 101, 109, 116, 125, 134, 144, 154, 165, 177, 190, 203, 218, 233, 250, 268, 287, 308, 330, 354, 379, 406, 436, 467, 500, 536, 574, 616, 660, 707, 758, 812, 870, 932, 1000};
#else
const uint16_t lut[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 45, 46, 47, 48, 50, 51, 53, 54, 56, 57, 59, 60, 62, 64, 66, 67, 69, 71, 73, 75, 77, 80, 82, 84, 86, 89, 91, 94, 96, 99, 102, 105, 107, 110, 113, 117, 120, 123, 127, 130, 134, 137, 141, 145, 149, 153, 157, 162, 166, 171, 176, 180, 185, 191, 196, 201, 207, 212, 218, 224, 231, 237, 243, 250, 257, 264, 271, 279, 287, 294, 303, 311, 320, 328, 337, 347, 356, 366, 376, 386, 397, 408, 419, 431, 443, 455, 467, 480, 493, 507, 521, 535, 550, 565, 581, 597, 613, 630, 647, 665, 683, 702, 721, 741, 762, 783, 804, 826, 849, 872, 896, 921, 946, 972, 999};
#endif

uint16_t mb_cnt = 0;
uint8_t mb_start = 0;
uint8_t mb_done = 0;
uint16_t leds[3];


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	mb_start = 1;
	mb_cnt = 0;
	modbus_lib_append_data(UCA0RXBUF);
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR (void)
{
	if(mb_start != 0)
	{
		mb_cnt++;
		if(mb_cnt > 5)
		{
			mb_cnt = 0;
			mb_start = 0;
			mb_done = 1;
			__bic_SR_register_on_exit(CPUOFF);
		}
	}

}

// Modbus fcns

uint16_t modbus_lib_read_handler(uint16_t la){ // la: logical_address
    switch(la){
    	case 40001:
            return leds[0];
        case 40002:
            return leds[2];
        case 40003:
        	return leds[4];
        case 40004:
            return TA0CCR1;
        case 40005:
            return TA1CCR2;
        case 40006:
        	return TA1CCR1;
        default:
            return modbus_lib_send_error(MBUS_RESPONSE_ILLEGAL_DATA_ADDRESS);
    }
}

uint16_t modbus_lib_write_handler(uint16_t la, uint16_t value)
{
    switch(la)
    {
    	case 40001:
    		if(value < LED_STEPS)
    		{
    			leds[0] = value;
    			TA0CCR1 = lut[value];

    		}
    		break;
    	case 40002:
    		if(value < LED_STEPS)
    		{
    			leds[2] = value;
    			TA1CCR2 = lut[value];
    		}
    		break;
    	case 40003:
    		if(value < LED_STEPS)
    		{
    			leds[1] = value;
    			TA1CCR1 = lut[value];
    		}
    		break;

    	case 40004:
    		if(value < 1001)
    		{
    			leds[0] = 0xffff;
    			TA0CCR1 = value;
    		}
    		break;
    	case 40005:
    		if(value < 1001)
    		{
    			leds[2] = 0xffff;
    			TA1CCR2 = value;
    		}
    		break;
    	case 40006:
    		if(value < 1001)
    		{
    			leds[1] = 0xffff;
    			TA1CCR1 = value;
    		}
    		break;

    	default:
    		return MBUS_RESPONSE_ILLEGAL_DATA_ADDRESS;
    }
    return MBUS_RESPONSE_OK; // data is successfully written
}



int modbus_lib_transport_write(uint8_t* buffer, uint16_t length)
{
	for(uint16_t i=0; i<length; i++)
	{
		while(!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
		UCA0TXBUF = buffer[i];
	}
    return 0;
}





void init_pins(void)
{

	P1SEL = 0;
	P1SEL2 = 0;
	P1DIR = 0;

	P2SEL = 0;
	P2SEL2 = 0;
	P2DIR = 0;

	P1DIR |= BIT0; // Set bit0 in the I/O direction register to define as an output
	P1OUT &= ~BIT0;


	P2DIR |= BIT2; // Set P2.2 as output
	P2SEL |= BIT2; // Select output P2.2 to be TA1.1


	P2DIR |= BIT5; // Set P2.5 as output
	P2SEL |= BIT5; // Select output P2.5 to be TA1.2

	P2DIR |= BIT6; // Set P1.6 as output
	P2SEL |= BIT6; // Select output P1.6 to be TA0.1 // remap to P2.6 on 1.6 is i2c

	P1SEL |= BIT1 | BIT2; // P1.1 = RXD, P1.2=TXD
	P1SEL2 |= BIT1 | BIT2; // P1.1 = RXD, P1.2=TXD
	P1DIR |= BIT1 | BIT2;
}

void init_timers()
{
	TA0CTL = TASSEL_2 + MC_1 + ID_0; // SMCLK as input clock, count up to TA0CCR0, clock/1
	TA0CCR0 = 1000; // Set maximum count value to determine PWM frequency = SMCLK/ClockDivide/TACCR0 (1MHz/1/1000 = 1kHz)

	TA0CCTL1 = OUTMOD_7; // Set output to on when counter resets and off when counter equals TA0CCR1. Normal PWM.a
	TA0CCTL0 = CCIE;
	TA0CCR1 = 0;

	TA1CTL = TASSEL_2 + MC_1 + ID_0; // SMCLK as input clock, count up to TA0CCR0, clock/1
	TA1CCR0 = 1000; // Set maximum count value to determine PWM frequency = SMCLK/ClockDivide/TACCR0 (1MHz/1/1000 = 1kHz)

	TA1CCTL1 = OUTMOD_7; // Set output to on when counter resets and off when counter equals TA0CCR1. Normal PWM.a
	TA1CCR1 = 0;

	TA1CCTL2 = OUTMOD_7; // Set output to on when counter resets and off when counter equals TA0CCR1. Normal PWM.a
	TA1CCR2 = 0;
}

void init_uart(void)
{
	UCA0CTL1 |= UCSSEL_2 | UCSWRST; // SMCLK
	UCA0BR0 = 52; // 1MHz 115200
	UCA0BR1 = 0x00; // 1MHz 115200
	UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5
	UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
	UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
}


int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT
	DCOCTL = 0; // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ; // Set DCO
	DCOCTL = CALDCO_1MHZ;
	init_pins();
	init_uart();
	init_timers();

	ModbusConfig_t modbus_cfg =
	{
    	.address = MB_ADR
	};

	modbus_lib_init(&modbus_cfg);
	__bis_SR_register(GIE);

	while (1)
	{
		if(mb_done != 0)
		{
			modbus_lib_end_of_telegram();
			mb_done = 0;
		}
		__bis_SR_register(CPUOFF); // Enter LPM0 w/ int until Byte RXed
	}
}

