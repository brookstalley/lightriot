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

#include "include/tps92661_reader.h"
#include "include/tps92661_crc.h"
#include "tps92661.h"
#include <string.h>

static inline bool tps92661_reader_check_start(const tps92661_reader_t *reader) {
	return
		(reader->buffer[0] & 0b00000000) == TPS92661_FRAMETYPE__RESPONSE;
}

static inline bool tps92661_reader_check_size(const tps92661_reader_t *reader) {


	return reader->size == (size_t)(tps92661_reader_get_length(reader) + 3);
}

int tps92661_read(tps92661_t *device, uint8_t start_address, uint8_t *data, size_t data_length) {
	uint8_t message_type = 0;
	switch (data_length) {
	case 1:
		message_type = TPS92661_COMMAND__READ_1BYTE;
		break;
	case 2:
		message_type = TPS92661_COMMAND__READ_2BYTE;
		break;
	case 5:
		message_type = TPS92661_COMMAND__READ_5BYTE;
		break;
	case 10:
		message_type = TPS92661_COMMAND__READ_10BYTE;
		break;
	case 15:
		message_type = TPS92661_COMMAND__READ_15BYTE;
		break;
	default:
		return -1;
		// TODO: real error handling
	}
	// TODO: don't create a new writer with each call
	tps92661_writer_t pw;
	tps92661_writer_init(&pw, device->params.stream->buffer, device->params.stream->size);
	uart_half_duplex_set_tx(device->params.stream);

	tps92661_writer_read_make(&pw, device->params.id, start_address, message_type);
	uart_half_duplex_send(device->params.stream, pw.size);

	uart_half_duplex_set_rx(device->params.stream);

	// Response should be the expected data size plus responst type and CRC
	if (uart_half_duplex_recv(device->params.stream, data_length) != data_length + 3) {
		return TPS92661_TIMEOUT;
	}
	// TODO: Add CRC checking
	uint8_t  *response = &device->params.stream->buffer[1];
	memcpy(response, data, data_length);
	return 0;
}