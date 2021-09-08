// Lab10.c
// Runs on TM4C123
// Sing-Rong Chiu and Kai Lee
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/16/2021 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* 
 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// VCC   3.3V power to OLED
// GND   ground
// SCL   PD0 I2C clock (add 1.5k resistor from SCL to 3.3V)
// SDA   PD1 I2C data

//************WARNING***********
// The LaunchPad has PB7 connected to PD1, PB6 connected to PD0
// Option 1) do not use PB7 and PB6
// Option 2) remove 0-ohm resistors R9 R10 on LaunchPad
//******************************

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "SSD1306.h"
#include "Print.h"
#include "Random.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "TExaS.h"
#include "Switch.h"
//********************************************************************************
// debuging profile, pick up to 7 unused bits and send to Logic Analyzer
#define PB54                  (*((volatile uint32_t *)0x400050C0)) // bits 5-4
#define PF321                 (*((volatile uint32_t *)0x40025038)) // bits 3-1
// use for debugging profile
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PB5       (*((volatile uint32_t *)0x40005080)) 
#define PB4       (*((volatile uint32_t *)0x40005040)) 
// TExaSdisplay logic analyzer shows 7 bits 0,PB5,PB4,PF3,PF2,PF1,0 
// edit this to output which pins you use for profiling
// you can output up to 7 pins
void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|PF321|PB54; // sends at 10kHz
}
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}
// edit this to initialize which pins you use for profiling
void Profile_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x22;      // activate port B,F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 
  GPIO_PORTF_DEN_R |=  0x0E;   // enable digital I/O on PF3,2,1
  GPIO_PORTB_DIR_R |=  0x30;   // output on PB4 PB5
  GPIO_PORTB_DEN_R |=  0x30;   // enable on PB4 PB5  
}
//********************************************************************************
 
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

int main1(void){uint32_t time=0;
  DisableInterrupts();
  // pick one of the following three lines, all three set to 80 MHz
  //PLL_Init();                   // 1) call to have no TExaS debugging
  TExaS_Init(&LogicAnalyzerTask); // 2) call to activate logic analyzer
  //TExaS_Init(&ScopeTask);       // or 3) call to activate analog scope PD2
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear();   
  Random_Init(1);
  Profile_Init(); // PB5,PB4,PF3,PF2,PF1 
  SSD1306_ClearBuffer();
  SSD1306_DrawBMP(2, 62, SpaceInvadersMarquee, 0, SSD1306_WHITE);
  SSD1306_OutBuffer();
  EnableInterrupts();
  Delay100ms(20);
  SSD1306_ClearBuffer();
  SSD1306_DrawBMP(47, 63, PlayerShip0, 0, SSD1306_WHITE); // player ship bottom
  SSD1306_DrawBMP(53, 55, Bunker0, 0, SSD1306_WHITE);

  SSD1306_DrawBMP(0, 9, Alien10pointA, 0, SSD1306_WHITE);
  SSD1306_DrawBMP(20,9, Alien10pointB, 0, SSD1306_WHITE);
  SSD1306_DrawBMP(40, 9, Alien20pointA, 0, SSD1306_WHITE);
  SSD1306_DrawBMP(60, 9, Alien20pointB, 0, SSD1306_WHITE);
  SSD1306_DrawBMP(80, 9, Alien30pointA, 0, SSD1306_WHITE);
  SSD1306_DrawBMP(50, 19, AlienBossA, 0, SSD1306_WHITE);
  SSD1306_OutBuffer();
  Delay100ms(30);

  SSD1306_OutClear();  
  SSD1306_SetCursor(1, 1);
  SSD1306_OutString("GAME OVER");
  SSD1306_SetCursor(1, 2);
  SSD1306_OutString("Nice try,");
  SSD1306_SetCursor(1, 3);
  SSD1306_OutString("Earthling!");
  SSD1306_SetCursor(2, 4);
  while(1){
    Delay100ms(10);
    SSD1306_SetCursor(19,0);
    SSD1306_OutUDec2(time);
    time++;
    PF1 ^= 0x02;
  }
}

// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

typedef enum {English, Spanish} Language_t;
Language_t myLanguage=English;
typedef enum {LANGUAGE, START, SCORE, PAUSE} phrase_t;
const uint8_t Language_English[]="English";
const uint8_t Language_Spanish[]="Espa\xA4ol ";
const uint8_t Start_English[] ="Start!";
const uint8_t Start_Spanish[] ="Comienzo";
const uint8_t Score_English[]="Score:";
const uint8_t Score_Spanish[]="Puntaje:";
const uint8_t Pause_English[] ="-Paused-";
const uint8_t Pause_Spanish[] ="-Pausado-";
const uint8_t *Phrases[4][2]={
  {Language_English,Language_Spanish},
  {Start_English,Start_Spanish},
  {Score_English,Score_Spanish},
  {Pause_English,Pause_Spanish}
};

#define alive 1
#define dead 0

void LCD_Outfix(uint32_t data){};
void LCD_OutString(char *cm){};

extern uint32_t raw_x;
extern uint32_t raw_y;
int32_t timepassed;
uint8_t NeedToDraw;
int32_t Player_x;
int32_t Player_y;
uint16_t score;
uint16_t combo;
uint8_t myL;

int DeadAlive[133];
int Clicked[133];

struct sprite {
int32_t x;       // x coor 0 to 127
int32_t y;       // y coor 0 to 63
int32_t showuptime; // number of 50 ms intervals after start when it will show up
};

// x = [8, 95]
// y = [31,55]
typedef struct sprite STyp;
STyp Circles[64]={
{8, 40, 0}, {90, 40, 16}, {39, 40, 32}, {10, 35, 48}, {10, 45, 64}, {20, 50, 80}, {20, 31, 96}, {20, 40, 112}, {50, 40, 128}, {30, 40, 144}, {20, 55, 160}, {95, 31, 176}, {75, 31, 192}, {95, 40, 208}, {45, 40, 224}, {70, 40, 240}, {83, 49, 256}, {95, 34, 272}, {95, 55, 288}, {90, 31, 304}, {80, 37, 320},{50, 37, 336}, {70, 31, 352}, {90, 47, 368}, {63, 45, 384}, {43, 31, 400}, {50, 38, 416}, {50, 53, 432}, {10, 36, 448}, {10, 51, 464}, {17, 37, 480}, {35, 42, 496}, {20, 45, 512}, {40, 52, 528}, {70, 31, 544}, {25, 31, 560}, {84, 50, 576}, {95, 31, 592}, {95, 55, 608}, {25, 34, 624}, {38, 41, 640}, {74, 48, 656}, {60, 48, 672}, {92, 35, 688}, {70, 47, 704}, {60, 51, 720}, {90, 41, 736}, {90, 55, 752}, {17, 55, 768}, {35, 51, 784}, {83, 44, 800}, {24, 52, 816}, {62, 36, 832}, {90, 43, 848}, {80, 52, 864}, {60, 41, 880}, {36, 49, 896}, {90, 38, 912}, {90, 51, 928}, {31, 48, 944}, {15, 36, 960}, {80, 45, 976}, {16, 43, 992}, {30, 39, 1008}
};

void Move_Player(void){
	ADC_In();
	Player_x = ((117)*raw_x)/4096;
	Player_y = ((((63-9)*raw_y)/4096)+9);
	NeedToDraw = 1;
	}

int status;
void Update_Status(void){
	status = 0;
	while(status < 64){
		if(Circles[status].showuptime <= timepassed){ // sign should be changed to <= ? showuptime is 0 for first, so will always be less than timepassed
			if((timepassed - Circles[status].showuptime) < 32){
				if(Clicked[status] == 0){ 
						DeadAlive[status] = alive; // default changed to 1 since probably want to be alive at beginning of game
				}
			}

			if(Circles[status].showuptime > timepassed){
				DeadAlive[status] = dead;
			}

			if((timepassed - Circles[status].showuptime) > 32){
					DeadAlive[status] = dead;
			}
			
			if(Clicked[status] == 1){
					DeadAlive[status] = dead;
			}
		}
	status++;
	}
}

void Shoot(void){ 
	for(int i=0; i<64; i++){
		if(DeadAlive[i] == alive){
			if(24 <= (timepassed - Circles[i].showuptime) && (timepassed - Circles[i].showuptime) <= 32){
				if(-12 <= (Circles[i].x - Player_x) && (Circles[i].x - Player_x) <= 12){
					if(-12 <= (Circles[i].y - Player_y) && (Circles[i].y - Player_y) <= 12){

						DeadAlive[i] = dead;
						Clicked[i] = 1;
						NeedToDraw = 1;
						score += 1;
					}
				}
			}
		}
	}
}

int f;
void Draw(void){
SSD1306_ClearBuffer();
SSD1306_DrawBMP(Player_x, Player_y, Player, 0, SSD1306_INVERSE);
	f = 0;
	
while(f<64){
	if(DeadAlive[f] == alive){
		if(0 <= (timepassed - Circles[f].showuptime) && (timepassed - Circles[f].showuptime) < 8){
			SSD1306_DrawBMP((Circles[f].x-8), (Circles[f].y+8), CirclePointer4, 0, SSD1306_INVERSE);           
		}
		if((8 <= (timepassed - Circles[f].showuptime)) && ((timepassed - Circles[f].showuptime) < 16)){
			SSD1306_DrawBMP((Circles[f].x-6), (Circles[f].y+6), CirclePointer3, 0, SSD1306_INVERSE);      
		}
		if((16 <=(timepassed - Circles[f].showuptime)) && ((timepassed - Circles[f].showuptime) < 24)){
			SSD1306_DrawBMP((Circles[f].x-4), (Circles[f].y+4), CirclePointer2, 0, SSD1306_INVERSE);      
		}
		if((24 <= (timepassed - Circles[f].showuptime)) && ((timepassed - Circles[f].showuptime) < 32)){
			SSD1306_DrawBMP((Circles[f].x-2), (Circles[f].y+2), CirclePointer1, 0, SSD1306_INVERSE);      
		}
		SSD1306_DrawBMP(Circles[f].x, Circles[f].y, Circle, 0, SSD1306_INVERSE);
	}
	f++;
}

SSD1306_OutBuffer();      // takes 25 ms
NeedToDraw = 0;
}


void Pause(void){   
	// DisableInterrupts();
	while((GPIO_PORTE_DATA_R & 0x01) == 1){};
	SSD1306_OutClear();
	SSD1306_SetCursor(6, 6);
	SSD1306_OutString((char *)Phrases[PAUSE][myL]);
	while((GPIO_PORTE_DATA_R & 0x01) == 0){};
	while((GPIO_PORTE_DATA_R & 0x01) == 1){};    //falling edge
	Draw();
  // EnableInterrupts();	
}

void Buttons_Init(void){       
// PE0 Pause / Play button, PE1 Shoot button, both falling edge
	//  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E, already initialized in ADC init
	//  GPIO_PORTE_DIR_R &= ~0x03;	// (c) make PE0, PE1 in (built-in button)
	//  GPIO_PORTE_DEN_R |= 0x03; 	// 	enable digital I/O on PE0, PE1
	//  GPIO_PORTE_PUR_R |= 0x03; 	// 	enable weak pull-up on PE0, PE1
 /*
	GPIO_PORTE_IS_R &= ~0x03; 	// (d) PE0, PE1 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x03;	// 	PE0, PE1 is not both edges
  GPIO_PORTE_IEV_R |= 0x03;	// 	PE0, PE1 rising edge event
  GPIO_PORTE_ICR_R = 0x03;  	// (e) clear flag0, flag1
  GPIO_PORTE_IM_R |= 0x03;  	// (f) arm interrupt on PE0, PE1
  NVIC_PRI1_R = (NVIC_PRI7_R&0xFFFFFF1F)|0x00000020; // (g) priority 1 bits
  NVIC_EN0_R = 0x2C000000;      // (h) enable interrupt 20 in NVIC
	*/
	GPIO_PORTE_IS_R &= ~0x02; 	// (d) PE1 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x02;	// 	PE1 is not both edges
  GPIO_PORTE_IEV_R |= 0x02;	// 	PE1 rising edge event
  GPIO_PORTE_ICR_R = 0x02;  	// (e) clear flag1
  GPIO_PORTE_IM_R |= 0x02;  	// (f) arm interrupt on PE0, PE1
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF1F)|0x00000020; // (g) priority 1 bits
  NVIC_EN0_R = 0x2C000000;      // (h) enable interrupt 20 in NVIC
}

void GPIOPortE_Handler(void){
  GPIO_PORTE_ICR_R = 0x03;	  // acknowledge flag0, flag1
  if((GPIO_PORTE_DATA_R & 0x01) == 1){   // test for PE0
	Pause();
  }
  if((GPIO_PORTE_DATA_R & 0x02) == 1){   // test for PE1
	Shoot();
  }
}

void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF);
	NVIC_ST_CTRL_R = 7;
}

#define MS 80000

int main(void){
	for(int i=0; i<64; i++){
	DeadAlive[i]=0;
  Clicked[i]=0;
	}
	
  DisableInterrupts();
	PLL_Init();
//  TExaS_Init(&LogicAnalyzerTask);
	ADC_Init();  // turn on ADC, set channel to 5
	Buttons_Init();
	Sound_Init();
 // 32-point averaging
	SysTick_Init(5000000);    // 16 Hertz
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(30, 50, TitleScreen, 0, SSD1306_INVERSE);
	SSD1306_OutBuffer();

	//while ((GPIO_PORTE_DATA_R & 0x02) == 0){};
	//while ((GPIO_PORTE_DATA_R & 0x02) == 1){};
  // other initialization, like mailbox

  timepassed = 0;
	while((GPIO_PORTE_DATA_R & 0x02) == 0){};
	while((GPIO_PORTE_DATA_R & 0x02) == 0x02){};
	SSD1306_OutClear();
  while((GPIO_PORTE_DATA_R & 0x02) == 0){
	if ((GPIO_PORTE_DATA_R & 0x01) == 1){
		while((GPIO_PORTE_DATA_R & 0x01) == 1){};
		myL++;
		myL %= 2;
	}
	SSD1306_SetCursor(6, 3);
	SSD1306_OutString((char *)Phrases[LANGUAGE][myL]);
}
  while ((GPIO_PORTE_DATA_R & 0x02) == 1){};
	SSD1306_OutClear();
  SSD1306_OutString((char *)Phrases[START][myL]);
	
  EnableInterrupts();
 while(timepassed <= 1045){ 
	 Update_Status();
    if (NeedToDraw == 1){
			Draw();
		}
   }
   SSD1306_SetCursor(0, 0);
	 SSD1306_OutString((char *)Phrases[SCORE][myL]);
   SSD1306_OutUDec(score);
}
	
int whatever;
void SysTick_Handler(void){
	Move_Player();
	timepassed += 1; // (measures the time passed since the start of the game in 50 ms intervals) get circles to show up at set numbers of timepassed to be able to match to the song 
	if((GPIO_PORTE_DATA_R & 0x01) == 0x01){   // test for PE0
		Pause();
	}
	if((GPIO_PORTE_DATA_R & 0x02) == 0x02){   // test for PE1
		if (whatever == 0){
		Shoot();
		whatever = 1;
		}
	}
	else {
		whatever = 0;
	}
}
