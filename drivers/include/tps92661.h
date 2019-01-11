/*
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

 /**
  * @defgroup    drivers_tps92661 Dynamixel driver
  * @ingroup     drivers_actuators
  * @brief       Driver for TPS92661 LED matrix controller
  *
  * This chip provides shunt dimming of up to 12 LEDs
  *
  * @{
  *
  * @file
  * @brief       Interface definition for TPS92661 device driver
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  */

#ifndef TPS92661_H
#define TPS92661_H

#include <stdlib.h>
#include "tps92661_protocol.h"
#include "uart_half_duplex.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef uint8_t tps92661_id_t;    /**< device id type */

	/**
	 * @brief   Descriptor struct for a TPS92661 device
	 */
	typedef struct {
		uart_half_duplex_t *stream; /**< the stream used */
		tps92661_id_t id; /**< the device ID */
	} tps92661_t;

	/**
	 * @brief   Possible tps92661 return values
	 */
	enum {
		TPS92661_OK,               /**< Success */
		TPS92661_TIMEOUT,          /**< No response from the device */
		TPS92661_BUFFER_TOO_SMALL, /**< Buffer is too small for the message */
		TPS92661_INVALID_MESSAGE,  /**< Invalid message received */
	};

	/**
	 * @brief   Send a PING message to a device
	 *
	 * @param[in] stream   the stream
	 * @param[in] id       the device address
	 *
	 * @return             TPS92661_OK if a device answered
	 * @return             TPS92661_TIMEOUT if the device did not answer
	 * @return             TPS92661_BUFFER_TOO_SMALL if buffer is too small for the message
	 * @return             TPS92661_INVALID_MESSAGE if an invalid message was received
	 */
	int tps92661_ping(tps92661_t *device);

	/**
	 * @brief   Initialize a TPS92661 device
	 *
	 * @param[out] device  the TPS92661 device
	 * @param[in] stream   the stream
	 * @param[in] id       the device address
	 */
	void tps92661_init(tps92661_t *device, uart_half_duplex_t *stream, tps92661_id_t id);


#ifdef __cplusplus
}
#endif
#endif