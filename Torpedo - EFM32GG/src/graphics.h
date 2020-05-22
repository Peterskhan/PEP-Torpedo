#pragma once

// Board includes
#include <em_lcd.h>
#include <em_cmu.h>
#include <segmentlcd.h>
#include "../drivers/lcd/segmentlcd_individual.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// Project defines
#define LCD_SEGMENTS_COUNT        (13*7)
#define LCD_LOWER_SEGMENTS_COUNT  (13)

/**
 * Array of booleans indicating which segments of the lower
 * LCD display should be active at any given moment. Modifying
 * the content of this array directly results in the LCD display
 * changing. Changes take effect upon calling updateDisplay().
 */
bool displayMap[LCD_SEGMENTS_COUNT];

/**
 * Array of booleans indicating which segments of the lower
 * LCD display should be permanently active. A segment should
 * be permanently active if a ship is confirmed at that location.
 */
bool uncoveredMap[LCD_SEGMENTS_COUNT];

/**
 * Bitmask indicating the desired status of the spinner LCD segments.
 * Each bit of this variable directly corresponds with the specific
 * segments of the LCD. Modifying this value directly affects the
 * status of the spinner LCD. Changes take effect upon calling
 * updateDisplay().
 */
uint8_t spinnerBitmask;

/**
 * This variable holds the number of shots attempted by the user.
 * The application keeps track of already confirmed ships, trying
 * to fire on such does not increase the counter.
 */
uint8_t shotsTotal;

/**
 * @brief Initializes the LCD display, and the helper
 *        variables.
 */
void initDisplay(void);

/**
 * @brief Updates the lower part of the LCD display.
 */
void updateLowerLCD(void);

/**
 * @brief Updates the upper part of the LCD display.
 */
void updateUpperLCD(void);

/**
 * @brief Updates the spinner part of the LCD display.
 */
void updateSpinnerLCD(void);

/**
 * @brief Updates the whole LCD display by calling
 *        the update method of the different parts.
 */
void updateDisplay(void);

/**
 * @brief The FreeRTOS task that periodically updates
 *        the LCD displays of the EFM32GG board.
 * @param The FreeRTOS task parameter (unused).
 */
void prvGraphicsTask(void *prvParam);
