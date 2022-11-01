/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                 // define objects in main module
#include "osObjects.h"                  // RTOS object definitions
#include <stdio.h>
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Joystick.h"

/* Joystick pins:
   - center: P1_20 = GPIO1[20] ---- 4  P1.20
   - up:     P1_23 = GPIO1[23] ---- 8  P1.23
   - down:   P1_25 = GPIO1[25] ---- 10 P1.25
   - left:   P1_26 = GPIO1[26] ---- 1  P1.26
   - right:  P1_24 = GPIO1[24] ---- 2  P1.24
*/

//Round-Robin Threads
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

//Rate Monotonic Threads
void led_Thread1 (void const *argument); // thread function
void led_Thread2 (void const *argument); // thread function
void led_Thread3 (void const *argument); // thread function

osThreadDef (led_Thread1, osPriorityNormal, 1, 0);                // thread A
osThreadDef (led_Thread2, osPriorityAboveNormal, 1, 0);           // thread B
osThreadDef (led_Thread3, osPriorityHigh, 1, 0);  								// thread C

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

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	Joystick_Initialize ();
	LED_Initialize();
	
	uint32_t pos;
	pos= Joystick_GetState();
	
	if (pos == 8){ // Up on joystick = round-robin
		tid_Thread  = osThreadCreate (osThread(Thread1), NULL);
		tid2_Thread = osThreadCreate (osThread(Thread2), NULL);
		tid3_Thread = osThreadCreate (osThread(Thread3), NULL);
	}
	else if(pos == 2){ // Right on joystick
		osTimerId timer_0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)0);
		osTimerId timer_1 = osTimerCreate(osTimer(timer1_handle), osTimerPeriodic, (void *)1);
		osTimerId timer_2 = osTimerCreate(osTimer(timer2_handle), osTimerPeriodic, (void *)1);
		
		osTimerStart(timer_0, 400);	
		osTimerStart(timer_1, 400); 
		osTimerStart(timer_2, 200);
		
		T_led_ID1 = osThreadCreate(osThread(led_Thread1), NULL);
		T_led_ID2 = osThreadCreate(osThread(led_Thread2), NULL);
		T_led_ID3 = osThreadCreate(osThread(led_Thread3), NULL);
	}
	
  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}


void Thread1 (void const *argument) {
		int x = 0;
		for (x = 0; x < 15; x++) {
			counta++; 								// first thread increments count variable 15 times															
		}
		LED_On(1);		//P1.29
		osThreadJoin(Thread2);
}

void Thread2 (void const *argument) {
    for (i = 0; i < 4; i++) {
			v++;											// thread 2 increments a different variable only 4 times						 
			}
		osThreadJoin(Thread3);
}

void Thread3 (void const *argument) {
		int y = 0;
    for (y = 0; y < 4; y++) {   							
			z = z + 2;							// final thread adds 2 to itself 4 times (2x4)
		}   
		if (counta == 15){
			LED_On(2);		//P1.31
			}
		if (v == 4){
			LED_On(4);		//P2.3
			}
		if (z == 8){
			LED_On(0);		//P1.28
		}
}


void led_Thread1 (void const *argument) {
	for (;;) {
		LED_On(0);		//P1.28
		delay(20);			//20ms delay
		osSignalSet (T_led_ID1, 0x01);
		LED_Off(0); 
		osSignalWait (0x01,osWaitForever);
	}
}

void led_Thread2 (void const *argument) {
	for (;;) 	{
		LED_Off(1);	
		LED_On(1);		//P1.29
    delay(10);		//10ms delay
		LED_Off(1);
		osSignalWait (0x02,osWaitForever);
	}
}

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
		osSignalWait (0x03,osWaitForever);
	}
}
