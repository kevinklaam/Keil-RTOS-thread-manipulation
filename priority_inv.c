/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "Board_LED.h"                  // ::Board Support:LED
#include "RTE_Components.h"             // Component selection
#include "Board_Joystick.h"

void Thread_Mutex (void const *arg);                       // thread function
osThreadId tid_Thread_Mutex;                                    // thread id
osThreadDef (Thread_Mutex, osPriorityNormal, 1, 0);             // thread object

osMutexId mid_Thread_Mutex;                                     // mutex id
osMutexDef (SampleMutex);                                       // mutex name definition

void Thread_Semaphore (void const *argument);                   // thread function
osThreadId tid_Thread_Semaphore;                                // thread id
osThreadDef (Thread_Semaphore, osPriorityNormal, 1, 0);         // thread object

osSemaphoreId sid_Thread_Semaphore;                             // semaphore id
osSemaphoreDef (SampleSemaphore);                               // semaphore object

 
/*----------------------------------------------------------------------------
	CMSIS RTX Priority Inversion Example
	Priority Inversion = leave commented lines commented
	Priority Elevation = uncomment the 2 commented lines
	Anita Tino
*----------------------------------------------------------------------------*/

void P1 (void const *argument);
void P2 (void const *argument);
void P3 (void const *argument);

int i = 0;

osThreadDef(P1, osPriorityAboveNormal, 1, 0);
osThreadDef(P2, osPriorityNormal, 1, 0);
osThreadDef(P3, osPriorityBelowNormal, 1, 0);

osThreadId t_main,t_P1,t_P2,t_P3;

void delay(count){ 
	int k;
	for(k = count; k > 0; k--){
					;
			}
	}


void P1 (void const *argument) {
	for (;;) {
		
		uint32_t joy =  Joystick_GetState();
		printf("Joystick Pos %i \n", joy);
		
		
		if (joy == 2){
		printf("Entered LPT and About to take Mutex\n");
		Thread_Mutex("A");
		printf("Leaving LPT\n\n");
		delay(750);
		}
		
		if (joy == 16){
		printf("Entered LPT and About to take Semaphore\n");
		Thread_Semaphore("A");
		printf("Leaving LPT\n\n");
		delay(750);
		}
		
		else{
			printf("No joystick input\n");
			break;
		}

		osThreadYield (); 
	}
}
void P2 (void const *argument) {
	for(;;) {
		
		printf("Entered MPT\n");
		printf("Leaving MPT\n\n");
		delay(2000);
		osThreadYield (); 
	}
}

void P3 (void const *argument) {
	for(;;){
		
		uint32_t joy =  Joystick_GetState();
		printf("Joystick Pos %i \n", joy);
		
		if (joy == 2){
		printf("Entered LPT and About to take Mutex\n");
		Thread_Mutex("B");
		printf("Leaving LPT\n\n");
		delay(1000);
		}
		
		if (joy == 16){
		printf("Entered LPT and About to take Semaphore\n");
		Thread_Semaphore("B");
		printf("Leaving LPT\n\n");
		delay(1000);
		}
		
		else{
			printf("No joystick input\n");
			break;
		}
		
		osThreadYield (); 
	}
}

void Thread_Mutex (void const *arg) {
 
osMutexWait (mid_Thread_Mutex, NULL);
	printf("Accessing Mutex Shared Resource\n");
    osMutexRelease (mid_Thread_Mutex);
    osThreadYield ();            // suspend thread
  }

void Thread_Semaphore (void const *arg) {
 
osSemaphoreWait (sid_Thread_Semaphore, NULL);
	printf("Accessing Semaphore Shared Resource\n");
    osSemaphoreRelease (sid_Thread_Semaphore); 
    osThreadYield ();            // suspend thread
  }


int main(void)
{    
	osKernelInitialize (); 
	LED_Initialize();
	t_main = osThreadGetId ();
	osThreadSetPriority(t_main,osPriorityHigh);
	
	mid_Thread_Mutex = osMutexCreate (osMutex (SampleMutex));
  tid_Thread_Mutex = osThreadCreate (osThread(Thread_Mutex), NULL);
	printf("mutex created\n");
	
	sid_Thread_Semaphore = osSemaphoreCreate (osSemaphore(SampleSemaphore), 1);
  tid_Thread_Semaphore = osThreadCreate (osThread(Thread_Semaphore), NULL);
	printf("semaphore created\n");

	t_P3 = osThreadCreate(osThread(P3), NULL);
	//osDelay(5);
	t_P2 = osThreadCreate(osThread(P2), NULL);
	//osDelay(5);
	t_P1 = osThreadCreate(osThread(P1), NULL);
	

	osThreadTerminate(t_main);
	osKernelStart ();  
	
	for (;;) {}
}


