/*
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
  * @ingroup     boards_samd21-xpro
  * @{
  *
  * @file
  * @brief       Configuration of CPU peripherals for the Atmel SAM D21 Xplained
  *              Pro board
  *
  * @author      Travis Griggs <travisgriggs@gmail.com>
  * @author      Dan Evans <photonthunder@gmail.com>
  */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

//#define USB_CONFIG_VID 0x1209
//#define USB_CONFIG_PID 0x2004

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @name   External oscillator and clock configuration
	 *
	 * There are three choices for selection of CORECLOCK:
	 *
	 * - usage of the 48 MHz DFLL fed by external oscillator running at 32 kHz
	 * - usage of the PLL fed by the internal 8MHz oscillator divided by 8
	 * - usage of the internal 8MHz oscillator directly, divided by N if needed
	 *
	 *
	 * The PLL option allows for the usage of a wider frequency range and a more
	 * stable clock with less jitter. This is why this option is default.
	 *
	 * The target frequency is computed from the PLL multiplier and the PLL divisor.
	 * Use the following formula to compute your values:
	 *
	 * CORECLOCK = ((PLL_MUL + 1) * 1MHz) / PLL_DIV
	 *
	 * NOTE: The PLL circuit does not run with less than 32MHz while the maximum PLL
	 *       frequency is 96MHz. So PLL_MULL must be between 31 and 95!
	 *
	 *
	 * The internal Oscillator used directly can lead to a slightly better power
	 * efficiency to the cost of a less stable clock. Use this option when you know
	 * what you are doing! The actual core frequency is adjusted as follows:
	 *
	 * CORECLOCK = 8MHz / DIV
	 *
	 * NOTE: A core clock frequency below 1MHz is not recommended
	 *
	 * @{
	 */
#define CLOCK_USE_PLL       (1)

#if CLOCK_USE_PLL
	 /* edit these values to adjust the PLL output frequency */
#define CLOCK_PLL_MUL       (47U)               /* must be >= 31 & <= 95 */
#define CLOCK_PLL_DIV       (1U)                /* adjust to your needs */
/* generate the actual used core clock frequency */
#define CLOCK_CORECLOCK     (((CLOCK_PLL_MUL + 1) * 1000000U) / CLOCK_PLL_DIV)
#elif CLOCK_USE_XOSC32_DFLL
	 /* Settings for 32 kHz external oscillator and 48 MHz DFLL */
#define CLOCK_CORECLOCK     (48000000U)
#define CLOCK_XOSC32K       (32768UL)
#define CLOCK_8MHZ          (1)
#define GEN2_ULP32K         (1)
#else
	 /* edit this value to your needs */
#define CLOCK_DIV           (1U)
/* generate the actual core clock frequency */
#define CLOCK_CORECLOCK     (8000000 / CLOCK_DIV)
#endif
/** @} */

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC3,
        .irq            = TC3_IRQn,
        .pm_mask        = PM_APBCMASK_TC3,
        .gclk_ctrl      = GCLK_CLKCTRL_ID_TCC2_TC3,
#if CLOCK_USE_PLL || CLOCK_USE_XOSC32_DFLL
        .gclk_src       = GCLK_CLKCTRL_GEN(1),
        .prescaler      = TC_CTRLA_PRESCALER_DIV1,
#else
        .gclk_src       = GCLK_CLKCTRL_GEN(0),
        .prescaler      = TC_CTRLA_PRESCALER_DIV8,
#endif
        .flags          = TC_CTRLA_MODE_COUNT16,
    },
    {   /* Timer 1 */
        .dev            = TC4,
        .irq            = TC4_IRQn,
        .pm_mask        = PM_APBCMASK_TC4 | PM_APBCMASK_TC5,
        .gclk_ctrl      = GCLK_CLKCTRL_ID_TC4_TC5,
#if CLOCK_USE_PLL || CLOCK_USE_XOSC32_DFLL
        .gclk_src       = GCLK_CLKCTRL_GEN(1),
        .prescaler      = TC_CTRLA_PRESCALER_DIV1,
#else
        .gclk_src       = GCLK_CLKCTRL_GEN(0),
        .prescaler      = TC_CTRLA_PRESCALER_DIV8,
#endif
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

#define TIMER_0_MAX_VALUE   0xffff

/* interrupt function name mapping */
#define TIMER_0_ISR         isr_tc3
#define TIMER_1_ISR         isr_tc4

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */


/**
 * https://www.mouser.com/ds/2/268/Atmel-42223-SAM-R21_Datasheet-1065540.pdf
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] =  {
	{    // EXT2 -- mapping to UART that talks to TPS92661 
			.dev = &SERCOM3->USART,
			.rx_pin = GPIO_PIN(PA, 28),
			.tx_pin = GPIO_PIN(PA, 27),
			.mux = GPIO_MUX_F,
			.rx_pad = UART_PAD_RX_1,
			.tx_pad = UART_PAD_TX_0,
			.flags = UART_FLAG_NONE,
			.gclk_src = GCLK_CLKCTRL_GEN_GCLK0
	}
};

		/* Don't think USB actually goes here
		{    // EXT2 -- mapping to USB
			.dev    = &SERCOM4->USART,
			.rx_pin = GPIO_PIN(PA,28),
			.tx_pin = GPIO_PIN(PA,27),
			.mux    = GPIO_MUX_D,
			.rx_pad = UART_PAD_RX_1,
			.tx_pad = UART_PAD_TX_0,
			.flags  = UART_FLAG_NONE,
			.gclk_src = GCLK_CLKCTRL_GEN_GCLK0
		}
		{    // EXT2/3
			.dev    = &SERCOM4->USART,
			.rx_pin = GPIO_PIN(PB,11),
			.tx_pin = GPIO_PIN(PB,10),
			.mux    = GPIO_MUX_D,
			.rx_pad = UART_PAD_RX_3,
			.tx_pad = UART_PAD_TX_2,
			.flags  = UART_FLAG_NONE,
			.gclk_src = GCLK_CLKCTRL_GEN_GCLK0
		} */


	/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom5
#define UART_1_ISR          isr_sercom3
//#define UART_2_ISR          isr_sercom5

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_0_EN            1

#define PWM_MAX_CHANNELS    3
 /* for compatibility with test application */
#define PWM_0_CHANNELS      PWM_MAX_CHANNELS

/* PWM device configuration */

	static const pwm_conf_t pwm_config[] = {

	#if PWM_0_EN
		{TCC0, {
		/* GPIO pin, MUX value, TCC channel */
		{ GPIO_PIN(PA, 18), GPIO_MUX_F, 2 },
		{ GPIO_PIN(PA, 17), GPIO_MUX_F, 1 },
		{ GPIO_PIN(PA, 19), GPIO_MUX_F, 3 },
	}},
	#endif
	};

	/* number of devices that are actually defined */
#define PWM_NUMOF           (1U)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
 // From table 5-1 in https://www.mouser.com/ds/2/268/Atmel-42223-SAM-R21_Datasheet-1065540.pdf
	static const spi_conf_t spi_config[] = {
		{   // EXT1 
			// 
			.dev = &SERCOM2->SPI,
			.miso_pin = GPIO_PIN(PA, 15),
			.mosi_pin = GPIO_PIN(PA, 12),
			.clk_pin = GPIO_PIN(PA, 13),
			.miso_mux = GPIO_MUX_C,
			.mosi_mux = GPIO_MUX_C,
			.clk_mux = GPIO_MUX_C,
			.miso_pad = SPI_PAD_MISO_2,
			.mosi_pad = SPI_PAD_MOSI_0_SCK_1
		}
		/*,
		{   // EXT2
			.dev      = &SERCOM1->SPI,
			.miso_pin = GPIO_PIN(PA, 16),
			.mosi_pin = GPIO_PIN(PA, 18),
			.clk_pin  = GPIO_PIN(PA, 19),
			.miso_mux = GPIO_MUX_C,
			.mosi_mux = GPIO_MUX_C,
			.clk_mux  = GPIO_MUX_C,
			.miso_pad = SPI_PAD_MISO_0,
			.mosi_pad = SPI_PAD_MOSI_2_SCK_3
		},
		{   // EXT3
			.dev      = &SERCOM5->SPI,
			.miso_pin = GPIO_PIN(PB, 16),
			.mosi_pin = GPIO_PIN(PB, 22),
			.clk_pin  = GPIO_PIN(PB, 23),
			.miso_mux = GPIO_MUX_C,
			.mosi_mux = GPIO_MUX_D,
			.clk_mux  = GPIO_MUX_D,
			.miso_pad = SPI_PAD_MISO_0,
			.mosi_pad = SPI_PAD_MOSI_2_SCK_3
		}
		*/
	};

	/**
	 * @name SPI confguration
	 **/
#define SPI_CS_DAC = GPIO_PIN(PA, 22)
#define SPI_CS_MEM = GPIO_PIN(PA, 4)


	 //#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
	 /** @} */

	 /**
	  * @name I2C configuration
	  * @{
	  */
	  /*
	 #define I2C_NUMOF          (1U)
	 #define I2C_0_EN            1
	 #define I2C_1_EN            0
	 #define I2C_2_EN            0
	 #define I2C_3_EN            0
	 #define I2C_IRQ_PRIO        1

	 #define I2C_0_DEV           SERCOM2->I2CM
	 #define I2C_0_IRQ           SERCOM2_IRQn
	 #define I2C_0_ISR           isr_sercom2
	 // I2C 0 GCLK
	 #define I2C_0_GCLK_ID       SERCOM2_GCLK_ID_CORE
	 #define I2C_0_GCLK_ID_SLOW  SERCOM2_GCLK_ID_SLOW
	 // I2C 0 pin configuration
	 #define I2C_0_SDA           GPIO_PIN(PA, 8)
	 #define I2C_0_SCL           GPIO_PIN(PA, 9)
	 #define I2C_0_MUX           GPIO_MUX_D
	 */

	 /**
	  * @name RTC configuration
	  * @{
	  */
#define RTC_NUMOF           (1U)
#define RTC_DEV             RTC->MODE2
	  /** @} */

	  /**
	   * @name RTT configuration
	   * @{
	   */
#define RTT_NUMOF           (1U)
#define RTT_DEV             RTC->MODE0
#define RTT_IRQ             RTC_IRQn
#define RTT_IRQ_PRIO        10
#define RTT_ISR             isr_rtc
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (32768U)    /* in Hz. For changes see `rtt.c` */
#define RTT_RUNSTDBY        (1)         /* Keep RTT running in sleep states */
	   /** @} */

	   /**
		* @name ADC Configuration
		* @{
		*/
#define ADC_0_EN                           1
#define ADC_MAX_CHANNELS                   14
		/* ADC 0 device configuration */
#define ADC_0_DEV                          ADC
#define ADC_0_IRQ                          ADC_IRQn

/* ADC 0 Default values */
#define ADC_0_CLK_SOURCE                   0 /* GCLK_GENERATOR_0 */
#define ADC_0_PRESCALER                    ADC_CTRLB_PRESCALER_DIV512

#define ADC_0_NEG_INPUT                    ADC_INPUTCTRL_MUXNEG_GND
#define ADC_0_GAIN_FACTOR_DEFAULT          ADC_INPUTCTRL_GAIN_1X
#define ADC_0_REF_DEFAULT                  ADC_REFCTRL_REFSEL_INT1V

	static const adc_conf_chan_t adc_channels[] = {
		/* port, pin, muxpos */
		{GPIO_PIN(PA, 8), ADC_INPUTCTRL_MUXPOS_PIN16},      /* EXT1, pin 8 */
	 //   {GPIO_PIN(PB, 1), ADC_INPUTCTRL_MUXPOS_PIN9},      /* EXT1, pin 4 */
	 //   {GPIO_PIN(PA, 10), ADC_INPUTCTRL_MUXPOS_PIN18},    /* EXT2, pin 3 */
	 //   {GPIO_PIN(PA, 11), ADC_INPUTCTRL_MUXPOS_PIN19},    /* EXT2, pin 4 */
	 //   {GPIO_PIN(PA, 2), ADC_INPUTCTRL_MUXPOS_PIN0},      /* EXT3, pin 3 */
	//    {GPIO_PIN(PA, 3), ADC_INPUTCTRL_MUXPOS_PIN1}       /* EXT3, pin 4. This is
	 //                       disconnected by default. PA3 is connected to USB_ID.
	 //                       Move PA03 SELECT jumper to EXT3 to connect. */
	};

#define ADC_0_CHANNELS                     (1U)
#define ADC_NUMOF                          ADC_0_CHANNELS
	/** @} */

/**
 * @name USB peripheral configuration
 * @{
 */
static const sam0_common_usb_config_t sam_usbdev_config[] = {
    {
        .dm     = GPIO_PIN(PA, 24),
        .dp     = GPIO_PIN(PA, 25),
        .d_mux = GPIO_MUX_G,
        .device = &USB->DEVICE,
    }
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
