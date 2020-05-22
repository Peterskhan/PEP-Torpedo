#pragma once
#ifndef GAME_STATISTICS_H
#define GAME_STATISTICS_H

/*********************************************************************************
 * Copyright (c) 2020 Peter Gyulai, Balazs Zombó
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *********************************************************************************/

/*********************************************************************************
 * @file    game_statistics.h
 * @author  Peter Gyulai, Balazs Zombó
 * @version 1.0.0
 * @date    2020.05.21
 * @license	MIT License
 *
 * @brief   Game statistics task declaration and message type definitions.
 ********************************************************************************/

// Standard includes
#include <semaphore.h>
#include <stdint.h>


/**
 * @brief Defines the return code of a read error.
 */
#define READ_ERROR      (-1)

/**
 * @brief Defines the return code of a read timeout.
 */
#define READ_TIMEOUT    (-2)


/**
 * @brief Describes the possible message types.
 */
typedef enum MessageType {
    GameStartedMsg,
    GameFinishedMsg,
    SegmentSelectedMsg,
    SegmentFiredMsg,
    SegmentHitMsg,
    SegmentMissedMsg
} MessageType;

/**
 * @brief Describes the message sent when the game starts.
 */
typedef struct GameStartedMessage {
    uint32_t startTick;		/**< The value of the game tick counter when the game started. 	*/
    uint8_t  tickDelayMs;	/**< The time delay between game ticks in milliseconds. 		*/
    uint8_t  mapIndex;		/**< The index of the map the game is played on. 				*/
} GameStartedMessage;

/**
 * @brief Describes the message sent the the game ends.
 */
typedef struct GameFinishedMessage {
    uint32_t stopTick;		/**< The value of the game tick counter when the game finished.	*/
    uint8_t  shotsTotal;	/**< The total number of shots fired when the game finished.	*/
} GameFinishedMessage;

/**
 * @brief Describes the message sent when a new segment is selected.
 */
typedef struct SegmentSelectedMessage {
    uint32_t gameTick;		/**< The value of the game tick counter when the segment was selected.	*/
    uint8_t  segmentID;		/**< The ID of the segment selected. 									*/
} SegmentSelectedMessage;

/**
 * @brief Describes the message sent when a segment is fired at.
 */
typedef struct SegmentFiredMessage {
    uint32_t gameTick;		/**< The value of the game tick counter when the segment was fired at.	*/
    uint8_t  segmentID;		/**< The ID of the segment fired at. 									*/
} SegmentFiredMessage;

/**
 * @brief Describes the message sent when a segment is hit.
 */
typedef struct SegmentHitMessage {
    uint32_t gameTick;		/**< The value of the game tick counter when the segment was hit.	*/
    uint8_t  segmentID;		/**< The ID of the segment hit.	 									*/
} SegmentHitMessage;

/**
 * @brief Describes the message sent when a segment is missed.
 */
typedef struct SegmentMissedMessage {
    uint32_t gameTick;		/**< The value of the game tick counter when the segment was missed.	*/
    uint8_t  segmentID;		/**< The ID of the segment missed. 										*/
} SegmentMissedMessage;

/**
 * @brief This structure is used to pass multiple parameters to
 *        the game statistics task.
 */
struct statisticsParams {
    sem_t        *statisticsReleased;   /**< Mutex releasing the statistics task to proceed.        */
    volatile int *stopFlag;             /**< Flag indicating that the statistics task should stop.  */
};


/**
 * @brief  Reads one byte from the terminal with the specified timeout.
 * @param  [in] The file descriptor of the terminal.
 * @param  [in] The timeout value of the read.
 * @return The byte read or READ_ERROR / READ_TIMEOUT.
 */
int readByteFromTerminal(int terminalFileDescriptor, struct timeval timeout);

/**
 * @brief   Reads one game-started message from the terminal.
 * @param   The file descriptor of the terminal.
 * @returns Zero on success, -1 on failure.
 */
int readGameStartedMessage(int terminalFileDescriptor);

/**
 * @brief   Reads one game-finished message from the terminal.
 * @param   The file descriptor of the terminal.
 * @returns Zero on success, -1 on failure.
 */
int readGameFinishedMessage(int terminalFileDescriptor);

/**
 * @brief   Reads one segment-related message of the specified type.
 * @param   The file descriptor of the terminal.
 * @param   The type of the segment message.
 * @returns Zero on success, -1 on failure.
 */
int readSegmentMessage(int terminalFileDescriptor, MessageType type);

/**
 * @brief 	Task function that receives messages from the EFM32GG,
 * 			displays them on STDOUT and logs statistics.
 * @param	[in] The args param is a pointer to a statisticsParam
 *               structure - typecasted to void* - containing the
 *          release semaphore and the stop flag for the thread.
 * @returns NULL
 */
void* statisticsTaskFunction(void *args);

#endif // GAME_STATISTICS_H
