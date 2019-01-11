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
  * @brief       Interface definition for TPS92661 packet writer
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  */

#ifndef TPS92661_WRITER_H
#define TPS92661_WRITER_H

#include "tps92661_protocol.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
 * @brief TPS92661 packet writer struct
 */
	typedef struct {
		uint8_t *buffer; /**< data buffer */
		size_t size;     /**< packet's size */
		size_t limit;    /**< data buffer's size */
	} tps92661_writer_t;

	/**
	 * @brief Initialize the TPS92661 packet writer
	 *
	 * @param[out] writer    the packet writer
	 * @param[in] buffer     the buffer used to store data
	 * @param[in] limit      the size of the buffer (= maximum packet size)
	 */
	void tps92661_writer_init(tps92661_writer_t *writer, uint8_t *buffer, size_t limit);

	/**
	 * @brief Get the data buffer to send
	 *
	 * @param[out] writer    the packet writer
	 *
	 * @return the begining address of the buffer
	 */
	const uint8_t *tps92661_writer_get_data(const tps92661_writer_t *writer);

	/**
	 * @brief Get the data buffer's size to send
	 *
	 * @param[out] writer    the packet writer
	 *
	 * @return the buffer's size
	 */
	size_t tps92661_writer_get_size(const tps92661_writer_t *writer);

	/**
	 * @brief Build a WRITE packet
	 *
	 * @param[out] writer    the packet writer
	 * @param[in] id         the destination's id
	 * @param[in] reg        the register to write in
	 * @param[in] buffer     the data buffer to write
	 * @param[in] size       the data buffer's size
	 */
	void tps92661_writer_writeone_make(tps92661_writer_t *writer, uint8_t id, uint8_t reg_addr, uint8_t data);

	/**
	 * @brief Build a READ packet
	 *
	 * @param[out] writer    the packet writer
	 * @param[in] id         the destination's id
	 * @param[in] reg        the register to read
	 * @param[in] size       the size to read
	 */
	void tps92661_writer_readone_make(tps92661_writer_t *writer, uint8_t id, uint16_t reg_addr);


#ifdef __cplusplus
}
#endif

#endif /* TPS92661_WRITER_H */
/** @} */
