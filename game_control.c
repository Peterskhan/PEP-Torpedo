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
 * @file    game_control.c
 * @author  Peter Gyulai, Balazs Zombó
 * @version 1.0.0
 * @date    2020.05.21
 * @license	MIT License
 *
 * @brief   Game control task implementation.
 ********************************************************************************/

// Standard includes
#include <semaphore.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

// Project includes
#include "game_control.h"


/**
 * @brief   Task function that waits for STDIN to receive character
 *	        input, and forwards it to the EFM32GG.
 * @param   [in] The args param is a pointer to a controlParams
 *               structure - typecasted to void* - containing a
 *               pointer to the statisticsReleased semaphore,
 *               the terminal baudrate and port name.
 * @returns NULL
 */
void* controlTaskFunction(void *args) {

    // Status flag indicating if everything is allright
    uint8_t good = 1;

    // Extracting parameters
    struct controlParams* params = (struct controlParams*)(args);

    // Extracting the statisticsReleased semaphore from the arguments
    sem_t *statisticsReleased = params->statisticsReleased;

    // Extracting the baudrate
    uint32_t baudrate = params->speed;

    // Extracting the port name
    const char *portName = params->portName;

    // Setting up reading from STDIN
    struct termios input;

    // Reading current options of STDIN
    tcgetattr(STDIN_FILENO, &input);

    // Disabling canonical mode (for immeadiate reading)
    // and disabling echo (for console clarity)
    input.c_lflag &= ~ICANON;
    input.c_lflag &= ~ECHO;

    // Applying changes
    if(tcsetattr(STDIN_FILENO, TCSANOW, &input) == -1) {
        perror("Cannon set up STDIN");
        return NULL;
    }

    // Setting up writing to the EFM32
    struct termios terminal;
    int terminalFileDescriptor;

    // Clearing termios control structure
    memset(&terminal, 0, sizeof(struct termios));

    // Setting terminal options
    terminal.c_iflag = 0;
    terminal.c_oflag = 0;
    terminal.c_cflag = CS8 | CREAD | CLOCAL;
    terminal.c_lflag = 0;
    terminal.c_cc[VMIN] = 1;
    terminal.c_cc[VTIME] = 0;

    // Opening terminal
    if(good) terminalFileDescriptor = open(portName, O_WRONLY);
    if(terminalFileDescriptor == -1) {
        perror("Cannot open terminal");
        good = 0;
    }

    // Setting baud-rate
    cfsetospeed(&terminal, baudrate);
    cfsetispeed(&terminal, baudrate);

    // Applying changes
    if(good && tcsetattr(terminalFileDescriptor, TCSANOW, &terminal) == -1) {
        perror("Cannot set up terminal parameters");
        good = 0;
    }

    // Releasing the statistics thread to proceed after the terminal is initialized
    sem_post(statisticsReleased);

    // Repeat until stop is issued by sending the letter 'q'
    while(good) {

        // Preparing for reading STDIN via select(2)
        fd_set fileDescriptors;
        FD_ZERO(&fileDescriptors);
        FD_SET(STDIN_FILENO, &fileDescriptors);

        // Wait for a character to be ready on STDIN
        int status = select(STDIN_FILENO + 1, &fileDescriptors, NULL, NULL, NULL);

        // Handling errors of select(2) - timeout NOT used
        if(status == -1) {
            perror("The game control task has encountered an unexpected error "
                   "while waiting for STDIN input in select(2)");
            break;
        }

        // Read the next character from STDIN
        unsigned char c;
        if(read(STDIN_FILENO, &c, 1) == -1) {
            perror("The game control task has encountered an unexpected error "
                   "while reading from STDIN.");
            break;
        }

        // Check character for stop command
        if(c == 'q' || c == 'Q') break;

        // Forward the character to the EFM32GG
        if(write(terminalFileDescriptor, &c, 1) == -1) {
            perror("The game control task has encountered an unexpected error "
                   "while writing to the EFM32GG.");
            break;
        }
    }

    // Releasing resources
    close(terminalFileDescriptor);

    // Re-enabling canonical mode and echo
    input.c_lflag |= ICANON;
    input.c_lflag |= ECHO;

    // Applying changes
    if(tcsetattr(STDIN_FILENO, TCSANOW, &input) == -1) {
        perror("The game control task has encountered an unexpected error "
               "while restoring STDIN configuration.");
    }

    // Return NULL to join with the main thread
    return NULL;
}
