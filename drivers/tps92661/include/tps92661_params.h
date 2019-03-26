/*
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

 /**
  * @ingroup     drivers_tps92661
  *
  * @{
  *
  * @file
  * @brief       Default parameters for tps92661 LED matrix controller
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  */
#include "board.h"
#include "tps926661.h"

#ifndef TPS92661_PARAM_UART
#define TPS92661_PARAM_UART		(1)
#endif

#ifndef TPS92661_PARAM_ID
#define TPS92661_PARAM_ID		(0)
#endif

#ifndef TPS92661_ENABLE_PIN
#define TPS92661_ENABLE_PIN		GPIO(PA,23)
#endif

#ifndef TPS92661_ENABLE_MODE
#define TPS92661_ENABLE_MODE	GPIO_OUT
#endif

#ifndef TPS92661_PARAMS
#define TPS92661_PARAMS		{ 
.uart = TPS92661_PARAM_UART,
.id = TPS92661_PARAM_ID,
.enable_pin = TPS92661_ENABLE_PIN,
.enable_mode = TPS92661_ENABLE_MODE
}
#endif

static const tps92661_params_t tps92661_params[]{
	TPS92661_PARAMS
}