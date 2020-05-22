#pragma once
#ifndef COMMAND_ARGS_H
#define COMMAND_ARGS_H

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
 * @file    command_args.h
 * @author  Peter Gyulai, Balazs Zombó
 * @version 1.0.0
 * @date    2020.05.22
 * @license	MIT License
 *
 * @brief   Command line argument parsing declarations and termios helpers.
 ********************************************************************************/

// Standard includes
#include <termios.h>
#include <stdint.h>


/**
 * @brief Defines the maximum length of the terinal port name.
 */
#define PORTNAME_MAX_LENGTH     (16)

/**
 * @brief This structure contains a serial port speed value
 *        pair for conversion between termios and normal
 *        speed definitions.
 */
struct BNUM_speed {
    uint32_t speed; /**< The normal speed value (baud-rate) */
    uint32_t bnum;  /**< The termios equivalent definition  */
};


/**
 * @brief Parses the specified command line arguments and sets the
 *        termios speed and serial port name values.
 * @param [in] The number of arguments.
 * @param [in] The string array of arguments.
 * @param [out] The termios speed value.
 * @param [out] The name of the serial port.
 */
void parseCommandLine(int argc, char*const* argv, uint32_t* speed, char *port);

/**
 * @brief  Returns the termios equivalent value of the specified baud-rate.
 * @param  [in] The normal baud-rate value (in bits/seconds).
 * @return The equivalent termios value or -1 when not supported.
 */
int64_t toTermiosSpeed(unsigned speed);

/**
 * @brief Prints the command line usage help to STDOUT.
 */
void printHelp(void);

#endif // COMMAND_ARGS_H
