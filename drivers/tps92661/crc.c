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

#include "tps92661_crc.h"
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

uint8_t reverse_byte(uint8_t byte)
{
	// First, swap the nibbles
	byte = (((byte & 0xF0) >> 4) | ((byte & 0x0F) << 4));
	// Then, swap bit pairs
	byte = (((byte & 0xCC) >> 2) | ((byte & 0x33) << 2));
	// Finally, swap adjacent bits
	byte = (((byte & 0xAA) >> 1) | ((byte & 0x55) << 1));
	// We should now be reversed (bit 0 <--> bit 7, bit 1 <--> bit 6, etc.)
	return byte;}

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