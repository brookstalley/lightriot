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

//#define USB_CONFIG_VID 0x1209
//#define USB_CONFIG_PID 0x2004

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
 * @name    AT86RF233 configuration
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PB, 15)


/** @} */

/**
 * @name    MOSFET RGB LED 
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
	typedef struct {
		uint32_t pwm_freq;
		uint16_t pwm_res;
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	} pwmrgb_t;


/**
 * @name SW0 (Button) pin definitions
 * @{
 */
 // Updated for encoder switch
#define ENC_SW_PORT				PORT->Group[PA]
#define ENC_SW_PIN				GPIO_PIN(PA, 7)
#define ENC_SW_MODE				GPIO_IN_PU

#define ENC_A_PORT				PORT->Group[PA]
#define ENC_A_PIN				GPIO_PIN(PA, 14)
#define ENC_A_MODE				GPIO_IN_PU // Not sure if this should be pull-up

#define ENC_B_PORT				PORT->Group[PB]
#define ENC_B_PIN				GPIO_PIN(PB, 23)
#define ENC_B_MODE				GPIO_IN_PU // Not sure if this should be pull-up

#define TPS92661_ENABLE_PIN		GPIO_PIN(PA, 23)
#define TPS92661_ENABLE_MODE	GPIO_OUT

/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

void rgbled_init(uint32_t freq, uint16_t res);

void rgbled_setcolor(uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
