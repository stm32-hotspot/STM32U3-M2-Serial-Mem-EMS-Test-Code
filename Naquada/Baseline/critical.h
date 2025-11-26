/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

#ifndef CRITICAL_HDR_H
#define CRITICAL_HDR_H

/* Critical section macros */
#define CRITICAL_SECT_DEFINE                 uint32_t primask = __get_PRIMASK();
#define CRITICAL_SECT_LOCK()                 __disable_irq()
#define CRITICAL_SECT_UNLOCK()               __set_PRIMASK(primask)

/* Assing result of cmd to var */
#define CRITICAL_SECT_EXECUTE_READ(var, cmd) CRITICAL_SECT_EXECUTE((var) = (cmd))

/* New defines to enter/exit critical section with interrupts */
#define CRITICAL_ENTER()                                                                                               \
    do {                                                                                                               \
        CRITICAL_SECT_DEFINE;                                                                                          \
        CRITICAL_SECT_LOCK();
#define CRITICAL_EXIT()                                                                                                \
    CRITICAL_SECT_UNLOCK();                                                                                            \
    }                                                                                                                  \
    while (0)
#define CRITICAL_SECT_EXECUTE(cmd)                                                                                     \
    do {                                                                                                               \
        CRITICAL_ENTER();                                                                                              \
        cmd;                                                                                                           \
        CRITICAL_EXIT();                                                                                               \
    } while (0)

// TODO: Have a conditional code to choose between 3 modes
// Critical section code removed 
// Normal implementation
// Add Statistics to use debug cycle counter and keep the largest found disabled interrupt time interval over time.
      
#endif