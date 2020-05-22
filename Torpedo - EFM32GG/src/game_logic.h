#pragma once

// Standard includes
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// Project includes
#include "statistics.h"
#include "graphics.h"
#include "input.h"

// Project defines

/**
 * This macro defines the number of predefined map configurations
 * used by the application. Modify this value accordingly, if you
 * add or remove map configurations.
 */
#define PREDEFINED_MAP_COUNT 	(16)

/**
 * This macro defines the time delay between game ticks in milliseconds.
 */
#define GAMEPLAY_TICK_DELAY_MS 	(10)

/**
 * Enumeration defining the possible game states. The application operates as
 * a state machine with these possible states. The transitions between states
 * are controlled by the main loop based on user input and time constraints.
 */
typedef enum {GameStart, SegmentSelect, SegmentFire, SegmentHit, GameOver} GameStates;

/**
 * This variable holds the current state of the application.
 * The current state is controlled by the main loop, the specifics
 * are stated above.
 */
GameStates gameState;

/**
 * This variable holds the index of the currently selected segment.
 * The selected segment can be changed by user input in the SegmentSelect
 * state.
 */
uint8_t selectedSegment;

/**
 * This variable holds the starting memory address of the used
 * game map. The map configurations are residing in static program
 * memory (defined in the generateMap() method).
 */
bool *gameplayMap;

/**
 * This variable holds the index of the gameplay map currently used.
 */
uint8_t gameplayMapIndex;

/**
 * This variable holds the number of remaining ship parts
 * before the game ends.
 */
uint8_t remainingShips;

/**
 * This variable holds the number of ticks elapsed from the game start.
 */
uint32_t gameTick;

// Main game loop:

/**
 * Updates the gameloop based on the current state,
 * based on the current input state.
 */
void updateGameLoop(void);

// Methods called in GameStart state:

/**
 * @brief  Initializes the application and its dependencies.
 * @detail This method gets called every time the game loop
 * 		   ticks when in GameStart state. The method changes
 * 		   the game state to SegmentSelect after it finished
 * 		   the initialization process.
 */
void gameStart(void);

/**
 * @brief Generates the gameplay map by randomly selecting
 *        from a set of predefined maps, and assigning the
 *        value of the global gameplayMap pointer.
 */
void generateMap(void);

// Methods called in SegmentSelect state:

/**
 *
 */
void segmentSelect(void);

/**
 * @brief  Moves the selected segment to the left.
 * @detail The selected segment is moved to the next digit
 *         on the left. If the currently selected segment
 *         is in the leftmost digit, it is moved to the
 *         rightmost position.
 */
void moveLeft(void);

/**
 * @brief  Moves the selected segment to the right.
 * @detail The selected segment is moved to the next digit
 *         on the right. If the currently selected segment
 *         is in the rightmost digit, it is moved to the
 *         leftmost position.
 */
void moveRight(void);

/**
 * @brief  Moves the selected segment to the next position in
 *         the current digit.
 * @detail This operation always keeps the selected segment in
 *         the current digit.
 */
void moveUp(void);

/**
 * @brief  Moves the selected segment to the previous position in
 *         the current digit.
 * @detail This operation always keeps the selected segment in
 *         the current digit.
 */void moveDown(void);

/**
 * @brief  Sets the game state to SegmentFire, and increases the
 *         total amount of shots if the currently selected segment
 *         is not identified as a ship yet.
 */
void fire(void);

// Methods called in SegmentFire state:

/**
 * @brief  Spins the spinner display one time, and transfers the
 *         application to the next state.
 * @detail If the currently selected
 *         segment contains a ship, the next state will be SegmentHit.
 *         Otherwise the state is transferred back to SegmentSelect.
 */
void segmentFire(void);

// Methods called in SegmentHit state:

/**
 * @brief Blinks the spinner display three times, and permanently
 *        reveals the ship hit by the shot.
 */
void segmentHit(void);

// Methods called in GameOver state:

/**
 * @brief Returns the game state to GameStart.
 */
void gameOver(void);

// FreeRTOS tasks

/**
 * @brief The FreeRTOS task that periodically updates
 *  	  the game loop.
 * @param The FreeRTOS task parameter (unused).
 */
void prvGameloopTask(void *prvParam);
