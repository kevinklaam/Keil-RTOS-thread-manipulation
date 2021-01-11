/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
 #include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "Board_LED.h"                  // ::Board Support:LED
#include "RTE_Components.h"             // Component selection
#include "Board_Joystick.h"


/*----------------------------------------------------------------------------
	CMSIS RTX Priority Inversion Example
	Priority Inversion = leave commented lines commented
	Priority Elevation = uncomment the 2 commented lines
	Anita Tino
*----------------------------------------------------------------------------*/

void P1 (void const *argument);
void P2 (void const *argument);
void P3 (void const *argument);

_Bool semaphore = false;

osThreadDef(P1, osPriorityHigh, 1, 0);
osThreadDef(P2, osPriorityNormal, 1, 0);
osThreadDef(P3, osPriorityBelowNormal, 1, 0);

osThreadId t_main,t_P1,t_P2,t_P3;

osMutexId mid_Thread_Mutex;                                     // mutex id
osMutexDef (SampleMutex);                                       // mutex name definition

osSemaphoreId sid_Thread_Semaphore;                             // semaphore id
osSemaphoreDef (SampleSemaphore);                               // semaphore object


void delay(){ 
	
	int k, count = 0;
	
	for(k = 0; k < 100000; k++){
					count++;
		//printf("%i\n", k );
			}
	}

void P1 (void const *argument) {
	for (;;) 
	{
		
		LED_On(0);
		
		delay(); //execute something, and after requires service from P3
		
		osSignalSet(t_P3,0x01);						//call P3 to finish the task
		osSignalWait(0x02,osWaitForever);			//Error => priority inversion, P2 will run instead
		
		LED_On(6);
		LED_Off(6);
	}
}

void P2 (void const *argument) {
	for (;;) 	{
		if (semaphore == true){
			printf("P2 Hit Bin Semaphore");
			osThreadSetPriority(t_P3, osPriorityHigh);			// push control back to P3
			break;
		}
		
		//delay();
		
		printf("P2 Mutex/Semaphore Hit");
		osMutexWait (mid_Thread_Mutex, osWaitForever);
		osSemaphoreWait (sid_Thread_Semaphore, osWaitForever);  
	
		LED_On(1); 
		LED_Off(1); 
		
		osMutexRelease (mid_Thread_Mutex);
		osSemaphoreRelease (sid_Thread_Semaphore);

		
	}
}

void P3 (void const *argument) {
	
		uint32_t joy =  Joystick_GetState();
		printf("Joystick P3 Pos %i \n", joy);

		for (;;) 	{
			
			if (joy == 2) {
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			printf("P3 Mutex Created \n");
			delay(); //do something
			osSignalWait(0x01,osWaitForever); 
			LED_Off(0); //critical function to be requested by P1
			osSignalSet(t_P1,0x02);	 

			}
			
			if (joy == 16) {
			osSemaphoreWait (sid_Thread_Semaphore, osWaitForever);   
			printf("P3 Semaphore Created \n");
			delay(); //do something
			osSignalWait(0x01,osWaitForever); 
			LED_Off(0); //critical function to be requested by P1
			osSignalSet(t_P1,0x02);	 
	
			}
			
			if (joy == 1) {
			semaphore = true;
			printf("P3 Bin Semaphore \n");
			delay(); //do something
			osSignalWait(0x01,osWaitForever); 
			LED_Off(0); //critical function to be requested by P1
			osSignalSet(t_P1,0x02);	
			if (semaphore == true){
				osThreadSetPriority(t_P3, osPriorityBelowNormal);
				}	
			}
			
			else {
			printf("Priority Inversion \n");
			delay(); //do something
				
			osSignalWait(0x01,osWaitForever); 
			LED_Off(0); //critical function to be requested by P1
			osSignalSet(t_P1,0x02);	 
			}
			
		}	
}


int main(void)
{    
	osKernelInitialize (); 
	LED_Initialize();
	t_main = osThreadGetId ();
	osThreadSetPriority(t_main,osPriorityHigh);
	
	mid_Thread_Mutex = osMutexCreate (osMutex (SampleMutex));
	printf("mutex created\n");
	sid_Thread_Semaphore = osSemaphoreCreate (osSemaphore(SampleSemaphore), 1);
	printf("semaphore created\n");
	
	t_P3 = osThreadCreate(osThread(P3), NULL);
	osDelay(5);
	t_P2 = osThreadCreate(osThread(P2), NULL);
	osDelay(5);
	t_P1 = osThreadCreate(osThread(P1), NULL);


	
	osThreadTerminate(t_main);
	 osKernelStart ();    
	for (;;) {}
}

