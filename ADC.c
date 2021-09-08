// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 1/16/2021
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
uint32_t raw_x;
uint32_t raw_y;

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5

void ADC_Init(void){ 
SYSCTL_RCGCADC_R |= 0x0001;  // 1) activate ADC0
	SYSCTL_RCGCGPIO_R |= 0x18;    // 2) activate clock for Port D and E
	__asm__ {
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
	}
//while((SYSCTL_PRGPIO_R&0x10) != 0x10){};  // 3) stabilization
	GPIO_PORTE_DIR_R &= ~0x33;      // 4) make PE4, PE5 input, make PE0, PE1 in
	GPIO_PORTD_DIR_R |= 0x03;	// make PD0, PD1 outputs for LCD
	GPIO_PORTE_AFSEL_R |= 0x30;    // 5) enable alternate function
	GPIO_PORTE_DEN_R &= ~0x30;       // 6) disable digital I/O on PE4, PE5
	GPIO_PORTE_DEN_R |= 0x03; 	// 	enable digital I/O on PE0, PE1
	GPIO_PORTE_AMSEL_R |= 0x30;      // 7) enable analog functionality on PE4, PE5
// Initialize ADC
	ADC0_PC_R &= ~0xF;    
	ADC0_PC_R |= 0x1;          // 8) configure for 125K samples/sec
	ADC0_SSPRI_R = 0x0123;  // 9) Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0xFFF7;  // 10) disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000;    // 11) seq3 is software trigger
	ADC0_SSMUX3_R &= ~0x000F;
	ADC0_SSMUX3_R += 9;	// 12) set channel
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 9;	     // 13) no TS0 D0, yes IE0 END0 (Ain9=PE4)
	ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0 
	ADC0_IM_R &= ~0x0008;	// 14) disable SS3 interrupts
	ADC0_ACTSS_R |= 0x0008;   // 15) enable sample sequencer 3
	ADC0_SAC_R &= 0x0005;
	ADC0_SAC_R |= 0x0005; // set to 32 point hardware averaging (measured 32 times then averaged)


SYSCTL_RCGCADC_R |= 0x0002;  // 1) activate ADC1
//while((SYSCTL_PRGPIO_R&0x10) != 0x10){};  // 3) stabilization
__asm__ {
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
	}
// Initialize ADC1
	ADC1_PC_R &= ~0xF;    
	ADC1_PC_R |= 0x1;          // 8) configure for 125K samples/sec
	ADC1_SSPRI_R = 0x0123;  // 9) Sequencer 3 is highest priority
	ADC1_ACTSS_R &= ~0xFFF7;  // 10) disable sample sequencer 3
	ADC1_EMUX_R &= ~0xF000;    // 11) seq3 is software trigger
	ADC1_SSMUX3_R &= ~0x000F;
	ADC1_SSMUX3_R += 9;	// 12) set channel
	ADC1_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 8;	     // 13) no TS0 D0, yes IE0 END0 (Ain8=PE5)
	ADC1_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0 
	ADC1_IM_R &= ~0x0008;	// 14) disable SS3 interrupts
	ADC1_ACTSS_R |= 0x0008;   // 15) enable sample sequencer 3
	ADC1_SAC_R &= 0x0005;
	ADC1_SAC_R |= 0x0005; // set to 32 point hardware averaging (measured 32 times then averaged)
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
void ADC_In(void){  
// reads both slide pots and returns the values in global variables raw_x and raw_y
  ADC0_PSSI_R = 0x0008;     // start ADC
	
  while((ADC0_RIS_R & 0x08)==0){};   // status (busy-wait)
	raw_x = ADC0_SSFIFO3_R & 0xFFF;   // read data
	ADC0_ISC_R = 0x0008;      // clear flag (writing to this automatically clears the flag in ADC0_RIS_R)

ADC1_PSSI_R = 0x0008;     // start ADC1
	
  while((ADC1_RIS_R & 0x08)==0){};   // status (busy-wait)
	raw_y = ADC1_SSFIFO3_R & 0xFFF;   // read data
	ADC1_ISC_R = 0x0008;      // clear flag (writing to this automatically clears the flag in ADC1_RIS_R)
}


