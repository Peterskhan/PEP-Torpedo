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
 * @file    game_statistics.c
 * @author  Peter Gyulai, Balazs Zombó
 * @version 1.0.0
 * @date    2020.05.21
 * @license	MIT License
 *
 * @brief   Game statistics task implementation.
 ********************************************************************************/

// Standard includes
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>


// Project includes
#include "game_statistics.h"


// Global statistics values

// The total number of shots fired
uint8_t shotsTotal = 0;

// The time delay between game ticks in milliseconds
uint8_t tickDelayMs = 0;

// The game tick value at the last segment hit event
uint32_t lastHitTick = 0;

// The total number of missed shots
uint32_t missTotal = 0;

// The game tick value at game start
uint32_t startTick = 0;

// The game tick value at game finish
uint32_t stopTick = 0;

// The index of the game map
uint8_t mapIndex = 0;

// The sum of time intervals between segment hit events
uint32_t sumHitTimes = 0;


/**
 * @brief  Reads one byte from the terminal with the specified timeout.
 * @param  [in] The file descriptor of the terminal.
 * @param  [in] The timeout value of the read.
 * @return The byte read or READ_ERROR / READ_TIMEOUT.
 */
int readByteFromTerminal(int terminalFileDescriptor, struct timeval timeout) {

    // Preparing for reading terminal via select(2)
    fd_set fileDescriptors;
    FD_ZERO(&fileDescriptors);
    FD_SET(terminalFileDescriptor, &fileDescriptors);

    // Wait for a character to be ready on the terminal
    int status = select(terminalFileDescriptor + 1, &fileDescriptors,
                        NULL, NULL, &timeout);

    // Handling errors of select(2)
    if(status == -1) {
        perror("The game statistics task has encountered an unexpected error "
               "while waiting for input in select(2)");
        return READ_ERROR;
    }

    // Checking timeout and skipping if necessary
    if(status == 0) return READ_TIMEOUT;

    // Read the next byte from terminal
    unsigned char c;
    if(read(terminalFileDescriptor, &c, 1) == -1) return READ_ERROR;

    // Returning byte read
    return c;
}

/**
 * @brief   Reads one game-started message from the terminal.
 * @param   The file descriptor of the terminal.
 * @returns Zero on success, -1 on failure.
 */
int readGameStartedMessage(int terminalFileDescriptor) {

    // Timeout between bytes of one message is 1ms
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    // Resetting global statistics
    shotsTotal = 0;
    tickDelayMs = 0;
    lastHitTick = 0;
    missTotal = 0;
    startTick = 0;
    stopTick = 0;
    sumHitTimes = 0;

    // Buffer for reading bytes from the terminal
    int bytes[4] = {0};

    // Reading bytes of startTick
    bytes[0] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[1] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[2] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[3] = readByteFromTerminal(terminalFileDescriptor, timeout);

    // Checking read errors
    if(bytes[0] != READ_ERROR && bytes[0] != READ_TIMEOUT &&
       bytes[1] != READ_ERROR && bytes[1] != READ_TIMEOUT &&
       bytes[2] != READ_ERROR && bytes[2] != READ_TIMEOUT &&
       bytes[3] != READ_ERROR && bytes[3] != READ_TIMEOUT)
    {
        // Saving startTick
        startTick |= bytes[0];
        startTick |= bytes[1] << 8;
        startTick |= bytes[2] << 16;
        startTick |= bytes[3] << 24;
    }
    else return -1;

    // Reading tickDelayMs
    bytes[0] = readByteFromTerminal(terminalFileDescriptor, timeout);
    if(bytes[0] != READ_ERROR && bytes[0] != READ_TIMEOUT) {
        tickDelayMs = bytes[0];
    }
    else return -1;

    // Reading mapIndex
    bytes[0] = readByteFromTerminal(terminalFileDescriptor, timeout);
    if(bytes[0] != READ_ERROR && bytes[0] != READ_TIMEOUT) {
        mapIndex = bytes[0];
    }
    else return -1;

    // Printing message information
    printf("[GAME_STARTED    ]: mapIndex = %u, startTick = %u, tickDelay = %u ms\n",
           mapIndex, startTick, tickDelayMs);
    return 0;
}

/**
 * @brief   Reads one game-finished message from the terminal.
 * @param   The file descriptor of the terminal.
 * @returns Zero on success, -1 on failure.
 */
int readGameFinishedMessage(int terminalFileDescriptor) {

    // Timeout between bytes of one message is 1ms
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    // Buffer for reading bytes from the terminal
    int bytes[4] = {0};

    // Reading bytes of stopTick
    bytes[0] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[1] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[2] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[3] = readByteFromTerminal(terminalFileDescriptor, timeout);

    // Checking read errors
    if(bytes[0] != READ_ERROR && bytes[0] != READ_TIMEOUT &&
       bytes[1] != READ_ERROR && bytes[1] != READ_TIMEOUT &&
       bytes[2] != READ_ERROR && bytes[2] != READ_TIMEOUT &&
       bytes[3] != READ_ERROR && bytes[3] != READ_TIMEOUT)
    {
        // Saving stopTick
        stopTick |= bytes[0];
        stopTick |= bytes[1] << 8;
        stopTick |= bytes[2] << 16;
        stopTick |= bytes[3] << 24;
    }
    else return -1;

    // Reading shotsTotal
    bytes[0] = readByteFromTerminal(terminalFileDescriptor, timeout);
    if(bytes[0] != READ_ERROR && bytes[0] != READ_TIMEOUT) {
        shotsTotal = bytes[0];
    }
    else return -1;

    // Printing message information
    printf("[GAME_FINISHED   ]: stopTick = %u, shotsTotal = %u\n\n",
           stopTick, shotsTotal);

    // Calculating statistics
    unsigned hitsTotal = shotsTotal - missTotal;
    double hitRate = 100.0f * hitsTotal / shotsTotal;
    double averageHitTime = ((double)sumHitTimes / hitsTotal) * tickDelayMs / 1000.0f;
    double gameTime = ((double)stopTick - startTick) / 1000.0f * tickDelayMs;

    // Printing statistics
    printf("STATISTICS:                    \n"
           "-------------------------------\n"
           "Shots total:     %u            \n"
           "Hits total:      %u            \n"
           "Misses total:    %u            \n"
           "Hitrate:         %.2lf%%       \n"
           "Average hittime: %.2lf seconds \n"
           "Game time:       %.2lf seconds \n\n\n",
           shotsTotal,
           hitsTotal,
           missTotal,
           hitRate,
           averageHitTime,
           gameTime
    );

    return 0;
}

/**
 * @brief   Reads one segment-related message of the specified type.
 * @param   The file descriptor of the terminal.
 * @param   The type of the segment message.
 * @returns Zero on success, -1 on failure.
 */
int readSegmentMessage(int terminalFileDescriptor, MessageType type) {

    // Timeout between bytes of one message is 1ms
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    // The value of the tick counter at segment message
    uint32_t gameTick = 0;

    // The ID of the segment
    uint8_t segmentID = 0;

    // Buffer for reading bytes from the terminal
    int bytes[4] = {0};

    // Reading bytes of gameTick
    bytes[0] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[1] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[2] = readByteFromTerminal(terminalFileDescriptor, timeout);
    bytes[3] = readByteFromTerminal(terminalFileDescriptor, timeout);

    // Checking read errors
    if(bytes[0] != READ_ERROR && bytes[0] != READ_TIMEOUT &&
       bytes[1] != READ_ERROR && bytes[1] != READ_TIMEOUT &&
       bytes[2] != READ_ERROR && bytes[2] != READ_TIMEOUT &&
       bytes[3] != READ_ERROR && bytes[3] != READ_TIMEOUT)
    {
        // Saving gameTick
        gameTick |= bytes[0];
        gameTick |= bytes[1] << 8;
        gameTick |= bytes[2] << 16;
        gameTick |= bytes[3] << 24;
    }
    else return -1;

    // Reading segmentID
    bytes[0] = readByteFromTerminal(terminalFileDescriptor, timeout);
    if(bytes[0] != READ_ERROR && bytes[0] != READ_TIMEOUT) {
        segmentID = bytes[0];
    }
    else return -1;

    // Differentiating based on message type
    switch(type) {
    case SegmentSelectedMsg:

        // Printing message information
        // printf("[SEGMENT_SELECTED]: segmentID = %u, gameTick = %u\n",
        //        segmentID, gameTick);
        break;

    case SegmentFiredMsg:

        // Printing message information
        printf("[SEGMENT_FIRED   ]: segmentID = %u, gameTick = %u\n",
               segmentID, gameTick);
        break;

    case SegmentHitMsg:

        // Printing message information
        printf("[SEGMENT_HIT     ]: segmentID = %u, gameTick = %u\n",
               segmentID, gameTick);

        // Updating last hit tick
        sumHitTimes += (gameTick - lastHitTick);
        lastHitTick = gameTick;

        break;

    case SegmentMissedMsg:

        // Printing message information
        printf("[SEGMENT_MISSED  ]: segmentID = %u, gameTick = %u\n",
               segmentID, gameTick);

        // Increasing total number of misses
        missTotal++;

        break;

    case GameStartedMsg:    // [[fallthrough]]
    case GameFinishedMsg:   // [[fallthrough]]
    default: break;
    };

    return 0;
}

/**
 * @brief 	Task function that receives messages from the EFM32GG,
 * 			displays them on STDOUT and logs statistics.
 * @param	[in] The args param is a pointer to a statisticsParam
 *               structure - typecasted to void* - containing the
 *          release semaphore and the stop flag for the thread.
 * @returns NULL
 */
void* statisticsTaskFunction(void *args) {

    // Typecasting the arguments to statisticsParam struct
    struct statisticsParams *params = (struct statisticsParams*)(args);

    // Extracting the stop flag from the thread arguments
    volatile int *stopFlag = params->stopFlag;

    // Extracting the task releasing semaphore from the arguments
    sem_t *statisticsReleased = params->statisticsReleased;

    // Waiting for the game control task to set up the terminal
    sem_wait(statisticsReleased);

    // Setting up reading from the EFM32GG
    // The terminal initialization is already performed
    // by the game control thread
    int terminalFileDescriptor;

    // Opening terminal
    terminalFileDescriptor = open("/dev/ttyACM0", O_RDONLY);
    if(terminalFileDescriptor == -1) {
        perror("Cannot open terminal");
        return NULL;
    }

    // Repeat until stop is requested by the stop flag
    while(*stopFlag == 0) {

        // Status flag
        int status = 0;

        // Creating 1s timeout between messages
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // Reading message identifier
        int messageID = readByteFromTerminal(terminalFileDescriptor, timeout);

        // Checking timeout and errors on read
        if(messageID == READ_TIMEOUT) continue;
        if(messageID == READ_ERROR) break;

        // Differentiating based on messageID
        switch(messageID) {
        case GameStartedMsg:
            status = readGameStartedMessage(terminalFileDescriptor);
            break;

        case GameFinishedMsg:
            status = readGameFinishedMessage(terminalFileDescriptor);
            break;

        case SegmentSelectedMsg:
            status = readSegmentMessage(terminalFileDescriptor, SegmentSelectedMsg);
            break;

        case SegmentFiredMsg:
            status = readSegmentMessage(terminalFileDescriptor, SegmentFiredMsg);
            break;

        case SegmentHitMsg:
            status = readSegmentMessage(terminalFileDescriptor, SegmentHitMsg);
            break;

        case SegmentMissedMsg:
            status = readSegmentMessage(terminalFileDescriptor, SegmentMissedMsg);
            break;

        default: status = -1; break;
        }

        // Checking message read error status
        if(status == -1) {
            fprintf(stderr, "The game statistics task has encountered an unexpected error      \n"
                            "while parsing messages from the EFM32GG. Please reset the device, \n"
                            "and restart the program.\n\n");
            break;
        }
    }

    // Releasing resources
    close(terminalFileDescriptor);

    return NULL;
}
