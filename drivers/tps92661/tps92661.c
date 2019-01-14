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
#include "periph/gpio.h"

#define LOW(v) (v & 0xFF)
#define HIGH(v) ((v >> 8) & 0xFF)

int tps92661_init(tps92661_t *device, tps92661_params_t *params)
{
	device->params.stream = params->stream;
	device->params.id = params->id;
	device->params.enable_pin = params->enable_pin;
	device->params.enable_mode = params->enable_mode;

	for (uint8_t i = 0; i < 12; i++) {
		device->current_power[i] = 0;
	}

	uint8_t pck_data = 0b0000011;
	tps92661_write(device, TPS92661_REG__PCKDIV, &pck_data, 1);

	uint8_t cfg_data = 0b0001111;
	tps92661_write(device, TPS92661_REG__SYSCFG, &cfg_data, 1);

	uint8_t def_led[2] = { 0x00, 0x00 };
	tps92661_write(device, TPS92661_REG__DEFLEDL, &def_led[0], 2);

	uint8_t enoff_data[2] = { 0b11111111, 0b00001111 };
	tps92661_write(device, TPS92661_REG__ENOFFL, &enoff_data[0], 2);

	gpio_clear(device->params.enable_pin);

	return tps92661_ping(device);
} 

int tps92661_ping(tps92661_t *device) {
	uint8_t syscfg_value;

	int result = tps92661_read(device, TPS92661_REG__SYSCFG, &syscfg_value, 1);

	if (result == TPS92661_TIMEOUT) {
		return TPS92661_TIMEOUT;
	}

	// TODO: print syscfg value or something

	return TPS92661_OK;
}

int tps92661_setchannel(tps92661_t *device, uint8_t channel, unsigned int power) {
	// power = 0 - 65535 and will be reduced to 10 bits to accommodate the device

	device->current_power[channel] = power;
	return 0;
}

void tps92661_recalc_channels(tps92661_t *device, uint8_t start_channel, uint8_t *channel_data) {
	// recalculate 4 channels of data. start_channel should always be 0, 4, or 8.
	// Target data should be 10 bytes long

	unsigned int start_time[4];
	unsigned int end_time[4];
	for (uint8_t i = 0; i < 4; i++) {
		// TODO: stagger values based on channel to smooth power usage
		if (device->current_power[i + start_channel] == 0) {
			// Special case: proper way to dim to 0 is to set PWM to 1 cycle and turn off enable: http://www.ti.com/lit/ds/symlink/tps92661-q1.pdf (page 18)
			start_time[i] = (start_channel << 6);
			end_time[i] = (start_channel << 6) + 1;
		}
		else if (device->current_power[i + start_channel] >= 65535) {
			start_time[i] = 0;
			end_time[i] = 1023;
		}
		else {
			start_time[i] = (start_channel << 6);
			// Fast integer ceiling
			end_time[i] = ((start_channel << 6) + device->current_power[i + start_channel] / 64 + (device->current_power[i + start_channel] % 64 != 0)) % 1024;
		}
	}
	channel_data[0] = LOW(start_time[0]);
	channel_data[1] = LOW(start_time[1]);
	channel_data[2] = LOW(start_time[2]);
	channel_data[3] = LOW(start_time[3]);
	channel_data[4] = LOW(end_time[0]);
	channel_data[5] = LOW(end_time[1]);
	channel_data[6] = LOW(end_time[2]);
	channel_data[7] = LOW(end_time[3]);
	channel_data[8] = HIGH(start_time[0]) << 6 | HIGH(start_time[1]) << 4 | HIGH(start_time[2]) << 2 | HIGH(start_time[3]);
	channel_data[9] = HIGH(end_time[0]) << 6 | HIGH(end_time[1]) << 4 | HIGH(end_time[2]) << 2 | HIGH(end_time[3]);
}

int tps92661_sendchannels(tps92661_t *device, uint8_t start_channel, uint8_t end_channel) {
	uint8_t channel_data[10];

	if (start_channel <= 3) {
		// We need to send channels 0-3
		tps92661_recalc_channels(device, 0, &channel_data[0]);
		tps92661_write(device, TPS92661_REG__MAPLED1ONL, &channel_data[0], 10);
	}
	if ((start_channel <= 7) && (end_channel >= 4)) {
		// We need to send channels 4-7
		tps92661_recalc_channels(device, 4, &channel_data[0]);
		tps92661_write(device, TPS92661_REG__MAPLED5ONL, &channel_data[0], 10);
	}
	if (end_channel >= 8) {
		// We need to send channels 8-11
		tps92661_recalc_channels(device, 8, &channel_data[0]);
		tps92661_write(device, TPS92661_REG__MAPLED9ONL, &channel_data[0], 10);
	}
	unsigned int enable_bits = 0;
	for (uint8_t i = 0; i < 12; i++) {
		enable_bits += device->current_power[i] == 0 ? 0 : 2 ^ i;
	}
	// Set ENON to 1 for each channel that has power > 0
	// ENOFF was set to 1 for all channels in init call
	uint8_t enon_data[2] = { LOW(enable_bits), HIGH(enable_bits) };
	tps92661_write(device, TPS92661_REG__ENONL, &enon_data[0], 2);

	return 0;
}

// TODO: "send all channels" which can be accomplished with two 15 byte writes rather than 3 10 bytes.

