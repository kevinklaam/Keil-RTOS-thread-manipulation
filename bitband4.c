#include "LPC17xx.h"
#include <stdio.h>
#include "Board_Joystick.h" 
#include "Board_LED.h" 
#include "GPIO_LPC17xx.h" 

//------- ITM Stimulus Port definitions for printf ------------------- //
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
} 

//------- Delay ------------------- //

void timeDelay (uint32_t seconds){
	uint32_t apple = seconds*1000;
	uint32_t dec;
	while(apple > 0){
			for (dec = 10; dec > 0; dec--){
							}
		apple--;	
	}
}
//------------------------------------------------------------------- //

// Bit Band Macros used to calculate the alias address at run time
#define ADDRESS(x)    (*((volatile unsigned long *)(x)))
#define BitBand(x, y) 	ADDRESS(((unsigned long)(x) & 0xF0000000) | 0x02000000 |(((unsigned long)(x) & 0x000FFFFF) << 5) | ((y) << 2))


#define ADCR_Bit24   (*((volatile unsigned long *)0x42680060))
	
#define BIT_ADDR1	 	 (*((volatile unsigned long *)0x233806F0))
#define BIT_ADDR2	 	 (*((volatile unsigned long *)0x23380A88))

uint32_t joystick;
volatile unsigned long * bit;
volatile unsigned long * bit2;
	
int main(void){

	Joystick_Initialize();	
	LED_Initialize();
	
	while(1) {
		int r3;
		int r1 = 1, r2 = 0;
		r3 = Joystick_GetState();
		
		while(r2 <= 0x18){
			if((r1 - r2) > 0){
				r1 = r1 + 2;
				r2 = r1 + (r3*4);
				printf("r2 <= 0x18 \n");
				break;
			}
			else{
				r2 = r2 + 1;
				printf("r2 + 1\n");
				break;
			}
		}
		if (r3 == 2) {		//ITE method invoked
			 
				printf("Method: Function\n"); //gpio24
				bit = &BitBand(&LPC_GPIO1->FIOPIN, 28);	
				bit2 = &BitBand(&LPC_GPIO2->FIOPIN, 2);	
				*bit = 1;
				//timeDelay(120);
				*bit2 = 1;
				//timeDelay(120);
				*bit = 0;
				//timeDelay(120);
				*bit2 = 0;
				//timeDelay(120);
		}
			
		else if (r3 == 8){	//ITE method invoked
				printf("Method: Bitband\n"); //gpio23
				BIT_ADDR1 = 1;
				//timeDelay(120);
				BIT_ADDR2 = 1;
				//timeDelay(120);
				BIT_ADDR1 = 0;
				//timeDelay(120);
				BIT_ADDR2 = 0;
				//timeDelay(120);
		}
		else if (r3 == 16){	//ITE method invoked
				printf("Method: Bitmask\n"); //gpio25
				LPC_GPIO1->FIOPIN |= 1UL<<28;
				//timeDelay(120);
				LPC_GPIO2->FIOPIN |= 1UL<<2;
				//timeDelay(120);
				LPC_GPIO1->FIOPIN &= ~(1UL<<28);
				//timeDelay(120);
				LPC_GPIO2->FIOPIN &= ~(1UL<<2);
				//timeDelay(120);
		}		
	}
	return 0;
}
