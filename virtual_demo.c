/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                      // CMSIS RTOS header file
#include <stdio.h>
#include <math.h>
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Joystick.h"
#include "Board_ADC.h"

  
void led_Thread1 (void const *argument); // thread function
void led_Thread2 (void const *argument); // thread function
void led_Thread3 (void const *argument); // thread function


osThreadDef (led_Thread1, osPriorityNormal, 1, 0);                // thread A
osThreadDef (led_Thread2, osPriorityAboveNormal, 1, 0);           // thread B
osThreadDef (led_Thread3, osPriorityHigh, 1, 0);  								// thread C


/*###########################################################
	Virtual Timer declaration and call back method
############################################################*/


osThreadId T_led_ID1;
osThreadId T_led_ID2;	
osThreadId T_led_ID3;	

void delay(int x){
	int i, d;
	d= 20000*x;
	for (i=0;i<d;i++){}
}

// Toggle the LED associated with the timer
void callback(void const *param){
	switch( (uint32_t) param){
		case 0:
			osSignalSet	(T_led_ID2,0x02); //thread B
			osSignalSet	(T_led_ID1,0x01); //thread A
			break;
		case 1:
			osSignalSet	(T_led_ID3,0x03);	//thread C
		  break;
	}
}
osTimerDef(timer0_handle, callback);
osTimerDef(timer1_handle, callback);
osTimerDef(timer2_handle, callback);

//#############################################################


/*#############################################################
  Flash LED 0, signal to thread 2, wait for 3 to finish
*#############################################################*/
void led_Thread1 (void const *argument) {
	for (;;) {
		LED_On(0);		//P1.28
		delay(20);			//20ms delay
		osSignalSet (T_led_ID1, 0x01);
		LED_Off(0); 
		osSignalWait (0x01,osWaitForever);
	}
}

/*################################################################
  Flash LED 2, signal to thread 3, wait for thread 1 to finish
*################################################################*/
void led_Thread2 (void const *argument) {
	for (;;) 	{
		LED_Off(1);	
		LED_On(1);		//P1.29
    delay(10);		//10ms delay
	
		LED_Off(1);
		osSignalWait (0x02,osWaitForever);
	}
}


/*################################################################
  Flash LED 4, signal to thread 1, wait for thread 2 to finish
*################################################################*/
void led_Thread3 (void const *argument){
	for (;;) 	{
		int x = 0;
		int counta = 0;
		LED_On(4); 		//P2.3

		for (x = 0; x < 15; x++) {
			counta++; 								
			}
		if (counta == 15){
			LED_On(2);		//P1.31
			}
		delay(5); 		//5ms delay
		LED_Off(4);
		osSignalWait (0x03,osWaitForever);
	}
}


/*###################################################################
  Create and start threads
 *###################################################################*/
int main (void) {
	//Virtual timer create and start
	osTimerId timer_0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)0);
	osTimerId timer_1 = osTimerCreate(osTimer(timer1_handle), osTimerPeriodic, (void *)1);
	osTimerId timer_2 = osTimerCreate(osTimer(timer2_handle), osTimerPeriodic, (void *)1);

	osKernelInitialize ();
  LED_Initialize ();	
	
	osTimerStart(timer_0, 400);	
	osTimerStart(timer_1, 400); 
	osTimerStart(timer_2, 200);	
	

	//Signal and wait threads
	T_led_ID1 = osThreadCreate(osThread(led_Thread1), NULL);
	T_led_ID2 = osThreadCreate(osThread(led_Thread2), NULL);
  T_led_ID3 = osThreadCreate(osThread(led_Thread3), NULL);
	osKernelStart ();      
	osDelay(osWaitForever);  
	
	for(;;);
}
