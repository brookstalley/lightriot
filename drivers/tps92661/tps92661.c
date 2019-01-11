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
  * @brief       Driver implementation for TPS92661 LED matrix controller
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  *
  * @}
  */

#include "tps92661.h"
#include "tps92661_reader.h"
#include "tps92661_writer.h"
#include "tps92661_protocol.h"

void tps92661_init(tps92661_t *device, uart_half_duplex_t *stream, tps92661_id_t id)
{
	device->stream = stream;
	device->id = id;
} 

int tps92661_ping(tps92661_t *device)
{
	tps92661_writer_t pw;

	uart_half_duplex_set_tx(device->stream);
	tps92661_writer_init(&pw, device->stream->buffer, device->stream->size);
	tps92661_writer_read_make(&pw, device->id, TPS92661_REG__SYSCFG, TPS92661_COMMAND__READ_1BYTE);
	uart_half_duplex_send(device->stream, pw.size);

	uart_half_duplex_set_rx(device->stream);
	if (uart_half_duplex_recv(device->stream, 1) != 1) {
		return TPS92661_TIMEOUT;
	}

	return TPS92661_OK;
}