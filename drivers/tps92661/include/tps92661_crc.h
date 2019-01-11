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
  * @brief       Interface definition for tps92661 crc
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  */

#ifndef TPS92661_CRC_H
#define TPS92661_CRC_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

	uint16_t crc_16_ibm(const uint8_t *buffer, size_t size);

	uint8_t reverse_byte(uint8_t byte);

	bool crc_valid(uint8_t *rx_buf, size_t crc_start);


#ifdef __cplusplus
}
#endif

#endif /* TPS92661_CRC_H */
/** @} */