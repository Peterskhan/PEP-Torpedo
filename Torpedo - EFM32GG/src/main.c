// Board includes
#include <em_device.h>
#include <em_chip.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// Project includes
#include "game_logic.h"
#include "statistics.h"
#include "graphics.h"


int main(void)
{
	// Chip errata
	CHIP_Init();

	// Initializing dependencies
	initDisplay();
	initInput();

	// Creating the gameplay task
	xTaskCreate(prvGameloopTask, "GAMELOOP", configMINIMAL_STACK_SIZE, NULL, 20, NULL);

	// Creating the graphics task
	xTaskCreate(prvGraphicsTask, "DISPLAY", configMINIMAL_STACK_SIZE, NULL, 10, NULL);

	// Creating the statistics task
	xTaskCreate(prvStatisticsTask, "STATISTICS", configMINIMAL_STACK_SIZE, NULL, 30, NULL);

	// Starting the FreeRTOS scheduler
	vTaskStartScheduler();

	// Suppressing compiler warnings
	return 0;
}
