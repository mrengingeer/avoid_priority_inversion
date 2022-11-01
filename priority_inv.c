/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                       // CMSIS RTOS header file
#include <stdio.h>
#include <math.h>
#include "Board_LED.h"                  // ::Board Support:LED
#include "RTE_Components.h"             // Component selection
#include "Board_Joystick.h"
  
/*----------------------------------------------------------------------------
	CMSIS RTX Priority Inversion Example
	Priority Inversion = leave commented lines commented
	Priority Elevation = uncomment the 2 commented lines
*----------------------------------------------------------------------------*/

void thread_A (void const *argument);
void thread_B (void const *argument);
void thread_C (void const *argument);

osThreadDef(thread_A, osPriorityAboveNormal, 1, 0);
osThreadDef(thread_B, osPriorityNormal, 1, 0);
osThreadDef(thread_C, osPriorityBelowNormal, 1, 0);

osMutexDef (uart_mutex);    // Declare mutex
osMutexId  (uart_mutex_id); // Mutex ID

osSemaphoreId multiplex_id;
osSemaphoreDef(multiplex_id);

osThreadId t_main;
osThreadId t_A;
osThreadId t_B;
osThreadId t_C;

void delay(int x){
	int i, d;
	d= 20000*x;
	for (i=0;i<d;i++){}
}

void thread_A (void const *argument) {
	for (;;) 
	{
		delay(4); 															  				//does something, and after requires service from C
		//osThreadSetPriority(t_C, osPriorityHigh); 				//solution for solving priorty inversion
		osSemaphoreWait(multiplex_id,osWaitForever);
		osMutexWait(uart_mutex_id, osWaitForever);
		//osSignalSet(t_C,0x01);	
		//osSignalWait(0x02,osWaitForever);									//Error => priority inversion, B will run instead
		delay(4); 
		osMutexRelease(uart_mutex_id);
		osSemaphoreRelease(multiplex_id);
		//osThreadSetPriority(t_C,osPriorityBelowNormal); 	//solution for solving priorty inversion
	}
}

void thread_B (void const *argument) {
	for (;;) 	{
		osSemaphoreWait(multiplex_id,osWaitForever);
		osMutexWait(uart_mutex_id, osWaitForever);
		delay(4);
		osMutexRelease(uart_mutex_id);
		osSemaphoreRelease(multiplex_id);
	}
}

void thread_C (void const *argument) {
	for (;;) 	{
		//delay(4); 														//does something 		
		osSemaphoreWait(multiplex_id,osWaitForever);
		osMutexWait(uart_mutex_id, osWaitForever);
		//osSignalWait(0x01,osWaitForever);
		//osSignalSet(t_A,0x02);	 							//critical function to be requested by A
		delay(4);
		osMutexRelease(uart_mutex_id);
		osSemaphoreRelease(multiplex_id);
	}
}

int main(void)
{    
	osKernelInitialize (); 
	LED_Initialize();
	Joystick_Initialize ();
	
	uint32_t pos;
	pos= Joystick_GetState();
	
	if (pos == 8){ 	// Up on joystick = mutex
		uart_mutex_id = osMutexCreate(osMutex(uart_mutex));
		t_main = osThreadGetId ();
		osThreadSetPriority(t_main,osPriorityHigh);
		t_C = osThreadCreate(osThread(thread_C), NULL);
		osDelay(5);
		t_B = osThreadCreate(osThread(thread_B), NULL);
		osDelay(5);
		t_A = osThreadCreate(osThread(thread_A), NULL);
	}
	
	if (pos == 2){ 	// Up on joystick = semaphore
		multiplex_id = osSemaphoreCreate(osSemaphore(multiplex_id),1);
		t_main = osThreadGetId ();
		osThreadSetPriority(t_main,osPriorityHigh);
		t_C = osThreadCreate(osThread(thread_C), NULL);
		osDelay(5);
		t_B = osThreadCreate(osThread(thread_B), NULL);
		osDelay(5);
		t_A = osThreadCreate(osThread(thread_A), NULL);
	}
	
	osThreadTerminate(t_main);
	osKernelStart ();    
	for (;;) {}
}

