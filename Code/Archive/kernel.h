/*
 * kernel.h
 *
 *  Created on: Sep 11, 2016
 *      Author: KSSKOU001: Hermann
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "TM38/defines.h"
#include "TM38/tm_stm32f4_disco.h"
#include "TM38/tm_stm32f4_exti.h"

#include "ST080Utils.h"	// this is where the FreeRTOS stuff (functions and variable are too)
#include "task.h"
#include "composer.h"	//composer mode functionalities
//#include "freestyle.h" //freestyle mode functionalities not yet implemented
//#include "playback.h"  //playback mode functionalities not yet implemented

//function prototypes
void vTaskSuspend( xTaskHandle xTaskToSuspend );
void vTaskResumeFromISR( xTaskHandle xTaskToSuspend );
void detachLine(void);
void kernelInit(void);
void InterruptConfig(void);
void vKernelTask(void *pvparameters);
void InterruptReconfig(void);

// define task priorities
#define COMPOSER_TASK_PRIORITY 2
#define PLAYBACK_TASK_PRIORITY 2
#define FREESTYLE_TASK_PRIORITY 2

// define task stack sizes
#define COMPOSER_STACK_SIZE 128
#define PLAYBACK_STACK_SIZE 128
#define FREESTYLE_STACK_SIZE 128

/*
 * FreeRTOS initial configs
 */
void kernelInit(void) {

};


/*
 * Associate 3 GPIO_Pins to Three Interrupts
 * Template Taken from Tino composer
 */
void InterruptConfig(void)
{
	// pins being used for kernel
	// PD0, PD1, PD2
	bool fail = false;
	if (TM_EXTI_Attach(GPIOD, GPIO_Pin_0, TM_EXTI_Trigger_Rising) != TM_EXTI_Result_Ok){
		// PA0
		fail = true;
	} else if (TM_EXTI_Attach(GPIOD, GPIO_Pin_1, TM_EXTI_Trigger_Rising) != TM_EXTI_Result_Ok){
		// PA1
		fail = true;
	} else if (TM_EXTI_Attach(GPIOD, GPIO_Pin_2, TM_EXTI_Trigger_Rising) != TM_EXTI_Result_Ok){
		// PA2
		fail = true;

	if (fail){
		// TODO failed configuring interrupts. (FLASH LEDs or something)
	}

};

//	handles for controlling tasks
static xTaskHandle composer_Handle;
static xTaskHandle playback_Handle;
static xTaskHandle freestyle_Handle;
static int reconfig_flag = 0;

void vKernelTask(void *pvparameters) {

	//Create Tasks storing handles
	xTaskCreate(vComposerTask, (signed char * ) "Composer Task", COMPOSER_STACK_SIZE, NULL,
			COMPOSER_TASK_PRIORITY, &composer_Handle);

//	xTaskCreate(vPlaybackTask, (signed char * ) "Playback Task", PLAYBACK_STACK_SIZE,
//						NULL, PLAYBACK_TASK_PRIORITY, &playback_Handle);
//
//	xTaskCreate(vFreestyleTask, (signed char * ) "Freestyle Task", FREESTYLE_STACK_SIZE,
//				NULL, FREESTYLE_TASK_PRIORITY, &freestyle_Handle);

	// Use the handle to suspend the created tasks
	vTaskSuspend(composer_Handle);
	vTaskSuspend(playback_Handle);
	vTaskSuspend(freestyle_Handle);

	InterruptConfig();

	for (;;) {
//		reconfigure interrupts for mode buttons
		if(reconfig_flag) {
			reconfig_flag = 0;
			InterruptReconfig();
		}
	}
};

void detachLine(void){
	//	detach the external interrupt for all GPIO_Pins in used
	TM_EXTI_Detach(GPIO_Pin_0);
	TM_EXTI_Detach(GPIO_Pin_1);
	TM_EXTI_Detach(GPIO_Pin_2);
};

void InterruptReconfig(void) {
	detachLine();
	InterruptConfig();
};

/**
 * @brief  EXTI Global handler
 * @note   This function is called from TM EXTI library each time any interrupt occurs on EXTI line.
 * @note   With __weak parameter to prevent link errors if not defined by user.
 * @param  GPIO_Pin: GPIO Line where interrupt occurred so you can identify what to do
 * @retval None
 */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
//	detach the external interrupt for all GPIO_Pins in used
	detachLine();
//	update reconfig flag
	reconfig_flag = 1;

    switch(GPIO_Pin){
    /* Resume the composer mode */
    case GPIO_Pin_0:
    	/*Turn on Composer led */

    	 //Resume composer task
    	 vTaskResumeFromISR(composer_Handle);
    	 break;

    /* Resume the playback mode */
    case GPIO_Pin_1:
       	/*Turn on Composer led */

       	 //Resume playback task
       	 vTaskResumeFromISR(playback_Handle);
       	 break;

    case GPIO_Pin_2:
       	/*Turn on freestyle led */

       	 //Resume playback task
       	 vTaskResumeFromISR(freestyle_Handle);
       	 break;
    }
};

#endif /* KERNEL_H_ */
