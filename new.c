
#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "Board_LED.h"                  // ::Board Support:LED
#include "RTE_Components.h"             // Component selection
#include "Board_Joystick.h"
 
osMutexId stdio_mutex;
osMutexDef(stdio_mutex);
 
void notify(const char* name, int state) {
    osMutexWait(stdio_mutex, osWaitForever);
    printf("%s: %d\n\r", name, state);
    osMutexRelease(stdio_mutex);
}
 
void test_thread(void const *args) {
    while (true) {
        notify((const char*)args, 0); osDelay(1000);
        notify((const char*)args, 1); osDelay(1000);
    }
}
 
void t2(void const *argument) {test_thread("Th 2");}
osThreadDef(t2, osPriorityNormal, DEFAULT_STACK_SIZE);
 
void t3(void const *argument) {test_thread("Th 3");}
osThreadDef(t3, osPriorityNormal, DEFAULT_STACK_SIZE);
 
int main() {
    stdio_mutex = osMutexCreate(osMutex(stdio_mutex));
    
    osThreadCreate(osThread(t2), NULL);
    osThreadCreate(osThread(t3), NULL);
    
    test_thread((void *)"Th 1");
}