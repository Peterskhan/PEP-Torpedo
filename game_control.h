#pragma once
#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

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
 * @file    game_control.h
 * @author  Peter Gyulai, Balazs Zombó
 * @version 1.0.0
 * @date    2020.05.21
 * @license	MIT License
 *
 * @brief   Game control task declaration.
 ********************************************************************************/

// Standard includes
#include <stdint.h>


/**
 * @brief This structure is used to pass multiple parameters to
 *        the game control task.
 */
struct controlParams {
    sem_t        *statisticsReleased;   /**< Mutex releasing the statistics task to proceed.        */
    const char   *portName;             /**< The name of the terminal port.                         */
    uint32_t      speed;                /**< The baudrate value of the terminal (in termios value). */
};

/**
 * @brief   Task function that waits for STDIN to receive character
 *	        input, and forwards it to the EFM32GG.
 * @param   [in] The args param is NOT used.
 * @returns NULL
 */
void* controlTaskFunction(void *args);

#endif // GAME_CONTROL_H
