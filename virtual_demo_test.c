/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
 #include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stdio.h>
#include <math.h>
#include "Board_LED.h"                  // ::Board Support:LED

  
void led_Thread1 (void const *argument); // thread function
void led_Thread2 (void const *argument); // thread function
void led_Thread3 (void const *argument);


osThreadDef (led_Thread1, osPriorityNormal, 1, 0);                   // thread object
osThreadDef (led_Thread2, osPriorityNormal, 1, 0);                   // thread object
osThreadDef (led_Thread3, osPriorityNormal, 1, 0);  


/*###########################################################
	Virtual Timer declaration and call back method
############################################################*/


osThreadId T_led_ID1;
osThreadId T_led_ID2;	
osThreadId T_led_ID3;	

// Toggle the LED associated with the timer
void callback(void const *param){
	switch( (uint32_t) param){
		case 0:
			LED_On(6); LED_Off(7);
			osDelay(50);
		  LED_Off(6); LED_Off(7);
			break;
		case 1:
			LED_Off(6); LED_On(7);
		  osDelay(50);
		  LED_Off(6); LED_Off(7);
		  break;
	}
}
osTimerDef(timer0_handle, callback);
osTimerDef(timer1_handle, callback);

//#############################################################


/*#############################################################
  Flash LED 0, signal to thread 2, wait for 3 to finish
*#############################################################*/
void led_Thread1 (void const *argument) {
	for (;;) {
		LED_On(0);
		osDelay(50);
		osSignalSet	(T_led_ID2,0x01);
		LED_Off(0); 
		osSignalWait (0x03,osWaitForever);
	}
}

/*################################################################
  Flash LED 2, signal to thread 3, wait for thread 1 to finish
*################################################################*/
void led_Thread2 (void const *argument) {
	for (;;) 	{
		LED_Off(2);	
		osSignalWait (0x01,osWaitForever);
		LED_On(2);
    osDelay(50);		
		LED_Off(2);
		osSignalSet	(T_led_ID3,0x02);
	}
}


/*################################################################
  Flash LED 4, signal to thread 1, wait for thread 2 to finish
*################################################################*/
void led_Thread3 (void const *argument){
	for (;;) 	{
		LED_Off(4);
		osSignalWait (0x02,osWaitForever);
		LED_On(4); 
		osDelay(50);
		LED_Off(4);
		osSignalSet	(T_led_ID1,0x03);
		
	}
}


/*###################################################################
  Create and start threads
 *###################################################################*/
int main (void) {
		osKernelInitialize (); 
	//Virtual timer create and start
	osTimerId timer_0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)0);	
	osTimerId timer_1 = osTimerCreate(osTimer(timer1_handle), osTimerPeriodic, (void *)1);	
	
  LED_Initialize ();	
	osTimerStart(timer_0, 3000);	
	osTimerStart(timer_1, 1000);	
	
	//Signal and wait threads
	T_led_ID1 = osThreadCreate(osThread(led_Thread1), NULL);
	T_led_ID2 = osThreadCreate(osThread(led_Thread2), NULL);
  T_led_ID3 = osThreadCreate(osThread(led_Thread3), NULL);
	 osKernelStart ();      
	osDelay(osWaitForever);               						
	
	for (;;);
}
