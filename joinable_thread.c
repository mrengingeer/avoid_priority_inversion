/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                       // CMSIS RTOS header file

 unsigned int counta=0;
void Thread1 (void const *argument); // thread function
void Thread2 (void const *argument); // thread function
void Thread3 (void const *argument); // thread function

osThreadId tid_Thread; // thread id
osThreadDef (Thread1, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid2_Thread; // thread id
osThreadDef (Thread2, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid3_Thread; // thread id
osThreadDef (Thread3, osPriorityNormal, 1, 0);                   // thread object

int v = 0;
int z = 0;
int i = 0;

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  tid_Thread = osThreadCreate (osThread(Thread1), NULL);
	tid2_Thread = osThreadCreate (osThread(Thread2), NULL);
	tid3_Thread = osThreadCreate (osThread(Thread3), NULL);
  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}


void Thread1 (void const *argument) {
		int x = 0;
		for (x = 0; x < 15; x++) {
			counta++; 								// first thread increments \
																		count variable 15 times
		}
}

void Thread2 (void const *argument) {
    for (i = 0; i < 4; i++) {
			v++;											// thread 2 increments a different\
																	 variable only 4 times
			}                                          
}

void Thread3 (void const *argument) {
		int y = 0;
    for (y = 0; y < 4; y++) {   // final thread adds 2 to itself \
																		4 times (2x4)
			z = z + 2;
  }                                          
}
