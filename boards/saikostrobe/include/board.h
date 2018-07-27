/*
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_samd21-xpro Atmel SAM D21 Xplained Pro
 * @ingroup     boards
 * @brief       Support for the Atmel SAM D21 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM D21 Xplained Pro
 *              board
 *
 * @author      Travis Griggs <travisgriggs@gmail.com>
 * @author      Dan Evans <photonthunder@gmail.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   xtimer configuration
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(1)
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name 

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 18)
#define LED1_PIN            GPIO_PIN(PA, 17)
#define LED2_PIN            GPIO_PIN(PA, 19)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 18)
#define LED1_MASK           (1 << 17)
#define LED2_MASK           (1 << 19)

#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_ON             (LED_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_OFF            (LED_PORT.OUTSET.reg = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL.reg = LED1_MASK)

#define LED2_ON             (LED_PORT.OUTCLR.reg = LED2_MASK)
#define LED2_OFF            (LED_PORT.OUTSET.reg = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT.OUTTGL.reg = LED2_MASK)


/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
 // Updated for encoder switch
#define BTN0_PORT           PORT->Group[PA]
#define BTN0_PIN            GPIO_PIN(PA, 7)
#define BTN0_MODE           GPIO_IN_PU

#define ENCA_PORT           PORT->Group[PA]
#define ENCA_PIN            GPIO_PIN(PA, 14)
#define ENCA_MODE           GPIO_IN_PU // Not sure if this should be pull-up

#define ENCB_PORT           PORT->Group[PA]
#define ENCB_PIN            GPIO_PIN(PB, 23)
#define ENCB_MODE           GPIO_IN_PU // Not sure if this should be pull-up


/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
