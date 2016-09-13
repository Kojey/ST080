#include "kernel.h"

//// define task priorities
#define KERNEL_TASK_PRIORITY 1
//#define COMPOSER_TASK_PRIORITY 2
//#define PLAYBACK_TASK_PRIORITY 2
//#define FREESTYLE_TASK_PRIORITY 2
//
//// define task stack sizes
#define KERNEL_STACK_SIZE 128
//#define COMPOSER_STACK_SIZE 128
//#define PLAYBACK_STACK_SIZE 128
//#define FREESTYLE_STACK_SIZE 128

// ============================================================================
int main(void) {
	//Create kernel Task
	xTaskCreate(vComposerTask, (signed char * ) "Kernel Task", KERNEL_STACK_SIZE, NULL,
			COMPOSER_TASK_PRIORITY, NULL);

	vTaskStartScheduler(); // This should never return.
	// Will only get here if there was insufficient memory to create
	// the idle task.
	while(1);
}
