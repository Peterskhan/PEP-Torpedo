#pragma once

// Standard includes
#include <stdbool.h>

// Board includes
#include <em_usart.h>
#include <em_cmu.h>
#include <em_gpio.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "queue.h"


/**
 * The characters assigned for each valid input state. Modify these
 * values to change the keyboard configuration used for each action.
 */
#define ACTION_LEFT 	('a')
#define ACTION_RIGHT 	('d')
#define ACTION_UP 		('w')
#define ACTION_DOWN 	('s')
#define ACTION_FIRE 	(' ')

/**
 * Enumeration for the possible input states. The application either receives
 * no input (None), movement input (MoveX) or fire input (Fire). The input state
 * is set asynchronously via an UART_Rx interrupt, and processed by the main loop.
 */
typedef enum {None, MoveLeft, MoveRight, MoveUp, MoveDown, Fire} InputStates;

/**
 * Handle to the input queue used to store the current user input.
 * The value is produced by the UART_RX interrupt and consumed by
 * the gameloop task.
 */
QueueHandle_t inputQueue;

/**
 * @brief  Initializes the UART used for user input.
 * @detail The specified configuration:
 * 		   - Baudrate:     115200
 * 		   - Data bits:    8
 *		   - Oversampling: 16
 *		   - Parity bits:  0
 *		   - Stop bits:    1
 *		   - Asynchronous mode.
 */
void initInput();

/**
 * @brief  The interrupt request handler of the UART.
 * @detail This function is called every time the UART
 *         receives data. This function does NOT handle
 *         the event, only sets the global input state.
 */
void UART0_RX_IRQHandler(void);
