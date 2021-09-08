// Timer0.c
// Runs on LM4F120/TM4C123
// Use TIMER0 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// Last Modified: 3/6/2015 
// You can use this timer only if you learn how it works

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "Sound.h"
#include "../inc/tm4c123gh6pm.h"

uint32_t notetime;
int soundon;
int soundcount;
int lenpt;
int notept;

extern int tetrisseq[];
extern void Sound_Start(uint32_t count);
extern void Sound_Off(void);

int soundchange[] = {28,36,40,44,48,50,52,56,60,68,72,76,84,88,92,100,104,108,116,124,132,140,148,160,168,172,180,184,188,200,204,212,216,220,228,232,236,244,252,260,268,276,284,292,296,300,308,312,316,324,328,332,340,344,348,356,360,364,372,380,388,396,404,416,424,428,436,440,444,456,460,468,472,476,484,488,492,500,508,516,524,532,540,556,572,588,604,620,636,652,668,684,700,716,732,740,748,764,796,804,808,812,820,824,828,836,840,844,852,856,860,868,872,876,884,892,900,908,916,928,936,940,948,952,956,968,972,980,984,988,996,1000,1004,1012,1020,1028,1036,1044,0};

void userfunc(void){};
void (*PeriodicTask0)(void);  // user function

// ***************** Timer0_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer0_Init(void(*task)(void), uint32_t period){
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  PeriodicTask0 = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER0A timeout
  soundcount++;
	// start on 28
	if(soundchange[lenpt] != 0){
		if(soundcount == soundchange[lenpt]){
			if(soundon == 1){
				Sound_Off();
				soundon = 0;
			}
			if(soundcount >= 28 && soundcount != 148 && soundcount != 276 && soundcount != 404 && soundcount != 532 && soundcount != 916 && soundcount != 1044){
				Sound_Start(tetrisseq[notept]);
				notept++;
				soundon = 1;
			}
		lenpt++;
		}
	}
	else{
	TIMER0_CTL_R = 0x00000000; // disable timer
	}
  (*PeriodicTask0)();                // execute user task
}

