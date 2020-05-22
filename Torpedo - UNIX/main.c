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
 * @file    main.c
 * @author  Peter Gyulai, Balazs Zombó
 * @version 1.0.0
 * @date    2020.05.21
 * @license	MIT License
 *
 * @brief   Defines the entry point for the application.
 ********************************************************************************/

// Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// Project includes
#include "command_args.h"
#include "game_control.h"
#include "game_statistics.h"


/**
 * @brief Semaphore to release the game statistics thread after
 *        the terminal is configured by the game control thread.
 */
sem_t statisticsReleased;

/**
 * @brief   The entry point for the application.
 * @details Creates the game control and the statistics thread
 *          using pthread_create and waits for them to join.
 * @param   argc
 * @param   argv
 * @return  EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    // Command line parameters
    uint32_t speed = 0;
    char portName[PORTNAME_MAX_LENGTH + 1] = "";

    // Parsing command line
    parseCommandLine(argc, argv, &speed, portName);

    // Checking speed configuration
    if(speed == 0) exit(EXIT_FAILURE);

    // Checking port name configuration
    if(strlen(portName) == 0) exit(EXIT_FAILURE);

    // Status variable for checking return values
    int status;

    // Status indicating if the application should stop
    volatile int stopFlag = 0;

    // The thread executing the control task
    pthread_t controlTask;

    // The thread executing the statistics task
    pthread_t statisticsTask;

    // Initializing the statisticsReleased semaphore as binary (mutex)
    sem_init(&statisticsReleased, 0, 1);

    // Acquiring the statisticsReleased semaphore, it will be released
    // by the game control task when the terminal is configured
    sem_wait(&statisticsReleased);

    // Assembling parameters for the control task
    struct controlParams cParams;
    cParams.statisticsReleased = &statisticsReleased;
    cParams.speed = speed;
    cParams.portName = portName;

    // Creating the control task
    status = pthread_create(&controlTask, NULL, controlTaskFunction, (void*) &cParams);

    // Handling errors
    if(status != 0) {
        fprintf(stderr, "Error: The game control thread can not be created.\n");
        exit(EXIT_FAILURE);
    }

    // Assembling parameters for the statistics task
    struct statisticsParams sParams;
    sParams.statisticsReleased = &statisticsReleased;
    sParams.stopFlag = &stopFlag;

    // Creating the statistics task
    status = pthread_create(&statisticsTask, NULL, statisticsTaskFunction, (void*) &sParams);

    // Handling errors
    if(status != 0) {
        fprintf(stderr, "Error: The statistics thread can not be created.\n");
        exit(EXIT_FAILURE);
    }

    // Waiting for the control task to join the main thread
    pthread_join(controlTask, NULL);

    // Setting the stop flag for the statistics thread
    stopFlag = 1;

    // Waiting for the statistics task to join the main thread
    pthread_join(statisticsTask, NULL);

    // Releasing resources
    sem_destroy(&statisticsReleased);

    // Exit with success
    exit(EXIT_SUCCESS);
}
