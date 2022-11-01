#include "LPC17xx.h"
#include "cmsis_os.h"
//#include "RTL.h"
#include "Board_LED.h"

// define the three operational threads


void thr_At (void const *arg);
void thr_Bt (void const *arg);
void thr_Ct (void const *arg);


// thread definitions are completed now define it using os
osSemaphoreId download_more_ram;
osSemaphoreDef(download_more_ram);


osThreadDef(thr_At,osPriorityNormal, 1, 0); //last two are # of instances and stack size requirements
osThreadDef(thr_Bt,osPriorityAboveNormal,1,0);
osThreadDef(thr_Ct,osPriorityHigh,1,0);


// define the virtual timer and callback stuff
osThreadId TRA;
osThreadId TRB;
osThreadId TRC;


void t_s (void const *param){
	osSignalSet(TRB,0x01);
	osSignalSet(TRA,0x01);
	osSignalSet(TRC,0x01);
}


void thr_A (void const *param){
	osSignalSet(TRA,0x01);
}

void thr_B (void const *param){
	osSignalSet(TRB,0x01);
}

void thr_C (void const *param){
	osSignalSet(TRC,0x01);
}




// timer thread declaraionts
osTimerDef(tms, t_s);
osTimerDef(tm0, thr_A);
osTimerDef(tm1,thr_B);
osTimerDef(tm2,thr_C);


void thr_At( void const *param){
	int k=0;
	int b=0;
	for(;;){
		LED_On(1);
		LED_On(5);


		osSignalWait(0x01,osWaitForever); // each for = .073 s
		osThreadSetPriority(TRA,osPriorityRealtime);
		osSemaphoreWait(download_more_ram,osWaitForever);
		for(k =0;k<0xFFFF;k++)for(b =0;b<58;b++);
		k=0;
		b=0;
		osSemaphoreRelease(download_more_ram);
		osThreadSetPriority(TRA, osPriorityNormal);
		LED_Off(1);
	}


}


void thr_Bt( void const *param){
	int k =0;
	int b=0;
	for(;;){
		LED_On(2);


		osSignalWait(0x01, osWaitForever);
		osSemaphoreWait(download_more_ram, osWaitForever);
		
		for(k =0;k<0xFFFF;k++)for(b =0;b<28;b++);//for(b =0;b<0xFFFFFFF;b++);//for(c =0;c<0xFFFFFFF;c++);
		k=0;
		b=0;
		osSemaphoreRelease(download_more_ram);


		LED_On(6);
		LED_Off(2);
	}
}


void thr_Ct( void const *param){
	int k =0;
	int b=0;
	
	for(;;){
		LED_On(3);
		LED_On(7);


		osSignalWait(0x01,osWaitForever); // 3.943ms
		osThreadSetPriority(TRA,osPriorityRealtime);
		osSemaphoreWait(download_more_ram,osWaitForever);
		for(k =0;k<0xFFFF;k++)for(b =0;b<13;b++);//for(b =0;b<0xFFFFFFF;b++);//for(c =0;c<0xFFFFFFF;c++);
		k=0;
		b=0;
		osSemaphoreRelease(download_more_ram);
		osThreadSetPriority(TRA, osPriorityHigh);
		LED_Off(3);
	}
}


//now the main


int main(void){
	osTimerId tm_S = osTimerCreate(osTimer(tms),osTimerOnce,(void *)0); // note the last statement refers to passing


	osTimerId tm0 = osTimerCreate(osTimer(tm0),osTimerPeriodic,(void *)0); // note the last statement refers to passing
// parameters. we must pass the address of a variable if we want to use it. ie &var
	osTimerId tm1 = osTimerCreate(osTimer(tm1),osTimerPeriodic,(void *) 0);
	osTimerId tm2 = osTimerCreate(osTimer(tm2),osTimerPeriodic,(void *) 0);


// ciompleted timer declarations. now we must start the timers initi stuff


	LED_Initialize ();
	download_more_ram = osSemaphoreCreate(osSemaphore(download_more_ram),1); // I did not name these, blame teh lab partner


	osTimerStart(tm_S,1);
	osTimerStart(tm0,4000);
	osTimerStart(tm1,3000);
	osTimerStart(tm2,2000);


	TRA = osThreadCreate(osThread(thr_At),NULL);
	TRB = osThreadCreate(osThread(thr_Bt),NULL);
	TRC = osThreadCreate(osThread(thr_Ct),NULL);


	osKernelStart();
	osDelay(osWaitForever);
	for(;;);
}

