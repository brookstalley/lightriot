/*
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

 /**
  * @ingroup     drivers_tps92661
  * @{
  *
  * @file
  * @brief       tps92661 messages reader
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  *
  * @}
  */

#ifndef TPS92661_READER_H
#define TPS92661_READER_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


	/**
	 * @brief tps92661 packet reader struct
	 */
	typedef struct {
		const uint8_t *buffer; /**< data buffer */
		size_t size;           /**< data buffer's size */
	} tps92661_reader_t;

	/**
		 * @brief Initialize the tps92661 packet reader
		 *
		 * @param[out] reader    the packet reader
		 * @param[in] buffer     the buffer used to store data
		 * @param[in] size       the size of the buffer
	*/
	static inline void tps92661_reader_init(dynamixel_reader_t *reader, const uint8_t *buffer, size_t size)
	{
		reader->buffer = buffer;
		reader->size = size;
	}


	/**
	 * @brief Check if the packet is valid
	 *
	 * @param[in] reader      the packet reader
	 *
	 * @return true if the packet is valid
	 * @return false otherwise
	 */
	bool tps92661_reader_is_valid(const tps92661_reader_t *reader);


	/**
	 * @brief Get the packet's length field
	 *
	 * @param[in] reader      the packet reader
	 *
	 * @return the packet's length field
	 */
	static inline uint16_t tps92661_reader_get_length(const tps92661_reader_t *reader)
	{
		uint8_t response_type = reader->buffer[0] & 0b00000111;
		uint8_t response_size = 0;
		switch (response_type) {
		case TPS92661_RESPONSE__1BYTE:
			response_size = 1;
			break;
		case TPS92661_RESPONSE__2BYTE:
			response_size = 2;
			break;
		case TPS92661_RESPONSE__5BYTE:
			response_size = 5;
			break;
		case TPS92661_RESPONSE__10BYTE:
			response_size = 10;
			break;
		case TPS92661_RESPONSE__15BYTE:
			response_size = 15;
			break;

		}
		return response_size;
	}

	static inline bool tps92661_reader_check_start(const tps92661_reader_t *reader);

	static inline bool tps92661_reader_check_size(const tps92661_reader_t *reader);

	/**
	 * @brief Get the packet's payload (response)
	 *
	 * @param[in] reader      the packet reader
	 *
	 * @return the addess of the begining of the payload
	 */
	static inline const uint8_t *tps92661_reader_status_get_payload(const dynamixel_reader_t *reader)
	{
		return &reader->buffer[1];
	}


	/**
	 * @brief Get the packet's payload size (response)
	 *
	 * @param[in] reader      the packet reader
	 *
	 * @return the size of the payload
	 */
	static inline size_t tps92661_reader_status_get_payload_size(const tps92661_reader_t *reader)
	{
		return tps92661_reader_get_length(reader) - 3;
	}
#ifdef __cplusplus
}
#endif

#endif /* DYNAMIXEL_READER_H */
/** @} */