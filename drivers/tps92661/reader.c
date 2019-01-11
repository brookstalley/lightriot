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

#include "tps92661_reader.h"
#include "tps92662_crc.h"



static inline bool tps92661_reader_check_start(const tps92661_reader_t *reader) {
	return
		(reader->buffer[0] & 0b00000000) == TPS92661_FRAMETYPE__RESPONSE
}

static inline bool tps92661_reader_check_size(const tps92661_reader_t *reader) {


	return reader->size == (size_t)(tps92661_reader_get_length(reader) + 3);
}

