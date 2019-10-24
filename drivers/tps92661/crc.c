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
  * @brief       TPS92661 CRC computation
  *
  * @author      Brooks Talley <brooks@tangentry.com>
  *
  * @}
  */

#include "include/tps92661_crc.h"
#define LOW(v) (v & 0xFF)
#define HIGH(v) ((v >> 8) & 0xFF)

uint16_t crc_16_ibm(const uint8_t *buf, size_t len)
{
	uint16_t crc = 0;
	uint16_t i;
	while (len--) {
		crc ^= *buf++;
		for (i = 0; i < 8; i++) {
			crc = (crc >> 1) ^ ((crc & 1) ? 0xa001 : 0);
		}
	}
	return crc;
}

static unsigned char lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

uint8_t reverse_byte(uint8_t n) {
	// Reverse the top and bottom nibble then swap them.
	return (lookup[n & 0b1111] << 4) | lookup[n >> 4];
}

bool crc_valid(uint8_t *rx_buf, size_t crc_start)
{
	uint16_t crc_calc; // Calculated CRC
	uint8_t crc_msb, crc_lsb; // Individual bytes of calculated CRC
	// Calculate the CRC based on bytes received
	crc_calc = crc_16_ibm(rx_buf, crc_start);
	crc_lsb = LOW(crc_calc);
	crc_msb = HIGH(crc_calc);
	// Perform the bit reversal within each byte
	crc_msb = reverse_byte(crc_msb);
	crc_lsb = reverse_byte(crc_lsb);
	// Do they match?
	if ((*(rx_buf + crc_start) == crc_lsb) && (*(rx_buf + crc_start + 1) == crc_msb)) {
		return true;
	}
	else {
		return false;
	}
}
