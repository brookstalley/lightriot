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
  * @brief       TPS92661 messages writer
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  *
  * @}
  */

#include "tps92661_writer.h"
#include "tps92661_crc.h"

#include <string.h>

#define LOW(v) (v & 0xFF)
#define HIGH(v) ((v >> 8) & 0xFF)

void tps92661_writer_init(tps92661_writer_t *writer, uint8_t *buffer, size_t limit)
{
	writer->buffer = buffer;
	writer->size = 0;
	writer->limit = limit;
}

const uint8_t *tps92661_writer_get_data(const tps92661_writer_t *writer)
{
	return (const uint8_t*)writer->buffer;
}

size_t tps92661_writer_get_size(const tps92661_writer_t *writer)
{
	return writer->size;
}


void tps92661_writer_write_make(tps92661_writer_t *writer, uint8_t id, uint8_t reg_addr, uint8_t *data, uint8_t command_type)
{
	uint16_t crc;
	uint8_t data_length = 0;
	switch (command_type) {
	case TPS92661_COMMAND__WRITE_1DEVICE_1BYTE:
		data_length = 1;
		break;
	case TPS92661_COMMAND__WRITE_1DEVICE_2BYTE:
		data_length = 2;
		break;
	case TPS92661_COMMAND__WRITE_1DEVICE_5BYTE:
		data_length = 5;
		break;
	case TPS92661_COMMAND__WRITE_1DEVICE_10BYTE:
		data_length = 10;
		break;
	case TPS92661_COMMAND__WRITE_1DEVICE_15BYTE:
		data_length = 15;
		break;
	}
	const uint8_t packet_length = data_length + 4;

	if (packet_length <= writer->limit) {
		writer->size = packet_length;
		writer->buffer[0] = (TPS92661_FRAMETYPE__COMMAND | command_type | id);
		writer->buffer[1] = reg_addr;
		memcpy(&writer->buffer[2], data, data_length);

		crc = crc_16_ibm(writer->buffer, data_length + 2);
		writer->buffer[writer->size - 2] = LOW(crc);
		writer->buffer[writer->size - 1] = HIGH(crc);
	}
	else {
		writer->size = 0;
	}
}

void tps92661_writer_read_make(tps92661_writer_t *writer, uint8_t id, uint16_t reg_addr, uint8_t command_type)
{
	const size_t len = 4;
	uint16_t crc;

	if (len <= writer->limit) {
		writer->size = len;

		writer->buffer[0] = (TPS92661_FRAMETYPE__COMMAND | command_type | id);
		writer->buffer[1] = reg_addr;

		crc = crc_16_ibm(writer->buffer, 2);

		writer->buffer[2] = LOW(crc);
		writer->buffer[3] = HIGH(crc);
	}
	else {
		writer->size = 0;
	}
}

int tps92661_write(tps92661_t *device, uint8_t start_address, uint8_t *data, size_t data_length) {
	uint8_t message_type = 0;
	switch (data_length) {
	case 1:
		message_type = TPS92661_COMMAND__WRITE_1DEVICE_1BYTE;
		break;
	case 2:
		message_type = TPS92661_COMMAND__WRITE_1DEVICE_2BYTE;
		break;
	case 5:
		message_type = TPS92661_COMMAND__WRITE_1DEVICE_5BYTE;
		break;
	case 10:
		message_type = TPS92661_COMMAND__WRITE_1DEVICE_10BYTE;
		break;
	case 15:
		message_type = TPS92661_COMMAND__WRITE_1DEVICE_15BYTE;
		break;
	default:
		return -1;
		// TODO: real error handling
	}
	// TODO: don't create a new writer with each call
	tps92661_writer_t pw;
	tps92661_writer_init(&pw, device->params.stream->buffer, device->params.stream->size);
	uart_half_duplex_set_tx(device->params.stream);
	tps92661_writer_write_make(&pw, device->params.id, start_address, data, message_type);
	uart_half_duplex_send(device->params.stream, pw.size);
	return 0;
}