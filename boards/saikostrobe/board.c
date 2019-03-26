/*
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_saikostrobe
 * @{
 *
 * @file
 * @brief       Board specific implementations for the saikostrobe board
 *
 * @author      Brooks Talley <brooks@tangentry.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

static pwmrgb_t rgbled;

void rgbled_init(uint32_t freq, uint16_t res) {
	rgbled.red = 0;
	rgbled.green = 0;
	rgbled.blue = 0;

	rgbled.pwm_freq = pwm_init(PWM_DEV(0), PWM_LEFT, freq, res);
	rgbled.pwm_res = res;
}

void rgbled_setcolor(uint8_t red, uint8_t green, uint8_t blue) {
	rgbled.red = red;
	rgbled.green = green;
	rgbled.blue = blue;
	pwm_set(0, 0, rgbled.red);
	pwm_set(0, 1, rgbled.green);
	pwm_set(0, 2, rgbled.blue);
}

void board_init(void)
{
	rgbled_init(10000, 256);

    /* initialize the on-board button */
    gpio_init(ENC_SW_PIN, ENC_SW_MODE);

	/* initialize the matrix controller in disabled mode */
	gpio_init(TPS92661_ENABLE_PIN, TPS92661_ENABLE_MODE);

    /* initialize the CPU */
    cpu_init();
}
