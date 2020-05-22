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
 * @file    command_args.c
 * @author  Peter Gyulai, Balazs Zombó
 * @version 1.0.0
 * @date    2020.05.22
 * @license	MIT License
 *
 * @brief   Command line parsing implementation.
 ********************************************************************************/

// Standard includes
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Project includes
#include "command_args.h"


/**
 * @brief   This array contains the possible speed definitions
 *          that are supported by termios.
 * @details Constants from /usr/include/asm-generic/termbits.h
 *          Constants are octal in termbits.h
 */
struct BNUM_speed g_speed[] = {
    { 50 , B50 },
    { 75 , B75 },
    { 110 , B110 },
    { 134 , B134 },
    { 150 , B150 },
    { 200 , B200 },
    { 300 , B300 },
    { 600 , B600 },
    { 1200 , B1200 },
    { 1800 , B1800 },
    { 2400 , B2400 },
    { 4800 , B4800 },
    { 9600 , B9600 },
    { 19200 , B19200 },
    { 38400 , B38400 },
    { 57600 , B57600 },
    { 115200, B115200 },
    { 230400 , B230400 },
    { 460800 , B460800 },
    { 500000 , B500000 },
    { 576000 , B576000 },
    { 921600 , B921600 },
    { 1000000 , B1000000 },
    { 1152000 , B1152000 },
    { 1500000 , B1500000 },
    { 2000000 , B2000000 },
    { 2500000 , B2500000 },
    { 3000000 , B3000000 },
    { 3500000 , B3500000 },
    { 4000000 , B4000000 },
    { 0 , 0 } // Null termination, keep as the last element...
};

/**
 * @brief Parses the specified command line arguments and sets the
 *        termios speed and serial port name values.
 * @param [in] The number of arguments.
 * @param [in] The string array of arguments.
 * @param [out] The termios speed value.
 * @param [out] The name of the serial port.
 */
void parseCommandLine(int argc, char*const* argv, uint32_t* speed, char *port) {

    // The termios speed value
    int64_t termiosSpeed = 0;

    // Identifier of the current argument
    int opt = 0;

    // Parsing command line arguments
    while((opt = getopt(argc, argv, "hs:p:")) != -1) {
        switch(opt) {

        // Printing program help
        case 'h':
            printHelp();
            break;

        // Setting baud-rate
        case 's':

            // Converting to termios speed
            termiosSpeed = toTermiosSpeed(atoi(optarg));

            // Printing status information
            if(termiosSpeed == 0) {
                fprintf(stderr, "ERROR: The specified Baud-rate is NOT supported!\n");
            } else {
                printf("INFO: Setting Baud-rate to %d\n", atoi(optarg));
            }

            // Setting termios speed
            *speed = termiosSpeed;
            break;

        // Setting terminal port name
        case 'p':

            // Checking port name length
            if(strlen(optarg) > PORTNAME_MAX_LENGTH) {
                fprintf(stderr, "ERROR: The specified port name is too long!\n");
                break;
            } else {
                printf("INFO: Setting port name to \"%s\"\n", optarg);
            }

            // Copying argument to output buffer
            strcpy(port, optarg);
            break;

        default: break;
        };
    }
}

/**
 * @brief  Returns the termios equivalent value of the specified baud-rate.
 * @param  [in] The normal baud-rate value (in bits/seconds).
 * @return The equivalent termios value or 0 when not supported.
 */
int64_t toTermiosSpeed(unsigned speed) {

    // Iterator to index g_speed[]
    unsigned i;

    // Searching g_speed for the proper termios value
    for (i = 0; g_speed[i].speed != speed; i++)
    {
        // If reached the end of g_speed, the specified
        // value is not supported by termios
        if (g_speed[i].speed == 0) {
            return 0;
        }
    }

    // Returning termios equivalent value
    return g_speed[i].bnum;
}

/**
 * @brief Prints the command line usage help to STDOUT.
 */
void printHelp(void) {
    printf("                                                     \n"
           "Torpedo application help:                            \n"
           "-------------------------                            \n"
           "-h: Prints this help.                                \n"
           "-s <baudrate>: Sets the baudrate.                    \n"
           "-p <portname>: Sets the portname (eg. /dev/ttyACM0). \n"
           "                                                     \n"
           "Example usage:                                       \n"
           "sudo ./pep_hf_unix -p \"/dev/ttyACM0\" -s 115200   \n\n");
}


