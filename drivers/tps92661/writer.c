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


void tps92661_writer_writeone_make(tps92661_writer_t *writer, uint8_t id, uint8_t reg_addr, uint8_t data)
{
	const size_t len = 5;
	uint16_t crc;

	if (len <= writer->limit) {
		writer->size = len;
		writer->buffer[0] = (0x80 | id);
		writer->buffer[1] = reg_addr;
		writer->buffer[2] = data;
		crc = crc_16_ibm(writer->buffer, 3);
		writer->buffer[3] = LOW(crc);
		writer->buffer[4] = HIGH(crc);
	}
	else {
		writer->size = 0;

	}
}

void tps92661_writer_readone_make(tps92661_writer_t *writer, uint8_t id, uint16_t reg_addr)
{
	const size_t len = 4;
	uint16_t crc;

	if (len <= writer->limit) {
		writer->size = len;

		writer->buffer[0] = (0xc0 | id);
		writer->buffer[1] = reg_addr;

		crc = crc_16_ibm(writer->buffer, 2);

		writer->buffer[2] = LOW(crc);
		writer->buffer[3] = HIGH(crc);
	}
	else {
		writer->size = 0;
	}
}