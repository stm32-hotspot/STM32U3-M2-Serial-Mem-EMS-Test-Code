/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

/**
 *

 */
#ifndef PRINF_COLOR
#define PRINTF_COLOR

#include <stdio.h>

#define RESET          "\x1b[0m"

/* ANSI TEXT COLOR */
#define BLACK        "\x1b[30m"
#define RED          "\x1b[31m"
#define GREEN        "\x1b[32m"
#define YELLOW       "\x1b[33m"
#define BLUE         "\x1b[34m"
#define MAGENTA      "\x1b[35m"
#define CYAN         "\x1b[36m"
#define WHITE        "\x1b[37m"

/* ANSI BACKGROUND COLOR */
#define BG_BLACK   "\x1b[40m"
#define BG_RED     "\x1b[41m"
#define BG_GREEN   "\x1b[42m"
#define BG_YELLOW  "\x1b[43m"
#define BG_BLUE    "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN    "\x1b[46m"
#define BG_WHITE   "\x1b[47m"

/* ANSI STYLE */
#define BOLD         "\x1b[1m" //set yellow color instead ?!?
#define ITALIC       "\x1b[3m"
#define UNDERLINE    "\x1b[4m" //set underline + green color with some release of teraterm

/* ANSI COMMAND */
#define CLEAR_SCREEN "\033c"

/** Usage examples
printf("\n\r "UNDERLINE"STM32U3 Naquada board, M.2 connector memory test"RESET" \n\r\n\r");
printf("\n\r Safe to "BOLD"unplug"RESET" the M.2 memory board\n\r");
printf("\n\r Safe to "BLUE"plug"RESET" the M.2 memory board\n\r");
**/

/* Macro to redefine printf */
#define printf_info(...) printf("\n\r"BLUE"M.2 MEMORY BOARD TEST INFO  : "RESET""__VA_ARGS__)
#define printf_error(...) printf("\n\r"RED"M.2 MEMORY BOARD TEST ERROR : "RESET""__VA_ARGS__)  

#endif
