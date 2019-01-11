#include "tps92661.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "xtimer.h"
#include "msg.h"
#include "stdio_uart.h"

#include "board.h"
#include "periph/gpio.h"
#include "periph/cpuid.h"
#include "led.h"

#include "drivers/include/tps92661.h"

/* Removing while we get the simple stuff up and running
#include "net/gnrc/pktdump.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktdump.h"
*/

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

static kernel_pid_t blink_pid;
static char blink_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];

#define MAIN_QUEUE_SIZE (64U)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define LIGHT_QUEUE_SIZE (8U)
static msg_t light_msg_queue[LIGHT_QUEUE_SIZE];

#define BLINK_INTERVAL_DEFAULT (1000UL * US_PER_MS)
static unsigned long blink_interval = BLINK_INTERVAL_DEFAULT;

#define MSG_STOP_TIMER (0x0101)
#define MSG_CHANGE_INTERVAL (0x0102)

tps92661_t matrix;

uint8_t cpuid[CPUID_LEN];

static uint8_t tps92661_buffer[128];
static uart_half_duplex_t tps92661_stream;

static int hello_world(int argc, char **argv) {
	/* Suppress compiler errors */
	(void)argc;
	(void)argv;
	printf("hello world!\n");
	return 0;
}

static void toggle_led(void) {
	LED0_TOGGLE;
}

static int cmd_toggle_led(int argc, char **argv) {
	(void)argc;
	(void)argv;
	toggle_led();
	return 0;
}

void *timer_blink(void *arg) {
	bool timer_run = true;
	msg_t msg;

	(void)arg;
	msg_init_queue(light_msg_queue, LIGHT_QUEUE_SIZE);
	while (timer_run) {
		if (msg_try_receive(&msg) == 1) {
			if (msg.content.value == MSG_STOP_TIMER) {
				timer_run = false;
			}
			else if (msg.content.value == MSG_CHANGE_INTERVAL) {
				// Nothing for now because we'll automatically pick it up
				puts("Timer interval changed\n");
			}
		}
		else {
			puts("blink\n");
			toggle_led();
			xtimer_usleep(blink_interval);
		}
	}
	printf("Timer thread exiting");
	blink_pid = 0;
	return NULL;
}

static int cmd_start_timer(int argc, char **argv) {
	(void)argc;
	(void)argv;
	kernel_pid_t thread_pid;

	if (blink_pid != 0) {
		printf("Timer already running with pid %d", blink_pid);
		return 0;
	}
	thread_pid = thread_create(blink_stack, sizeof(blink_stack),
		THREAD_PRIORITY_MAIN - 1, 0, timer_blink, NULL, "timer_blink");
	if (thread_pid == -EINVAL) {
		(void)puts("Invalid parameters\n");
	}
	else if (thread_pid == -EOVERFLOW) {
		puts("Error creating timer thread\n");
	}
	else {
		blink_pid = thread_pid;
		printf("Timer started with pid %d\n", blink_pid);
	}
	return 0;
}

static int cmd_stop_timer(int argc, char **argv) {
	(void)argc;
	(void)argv;

	if (blink_pid == 0) {
		puts("Timer not running");
		return 0;
	}

	msg_t msg;

	msg.content.value = MSG_STOP_TIMER;
	if (msg_try_send(&msg, blink_pid) == 0) {
		puts("Receiver queue full. Queue:\n");
		msg_queue_print();
	}
	else {
		puts("Timer stopping\n");
	}
	return 0;
}

static int cmd_change_interval(int argc, char **argv) {
	(void)argc;
	(void)argv;

	unsigned long new_interval = 0;

	if (blink_pid == 0) {
		puts("Timer not running");
		return 1;
	}
	if (argc < 1) {
		printf("usage: %s <ms>\n", argv[0]);
		return 1;
	}
	new_interval = atoi(argv[1]);
	if (new_interval <= 0) {
		puts("Must specify a positive integer for milliseconds");
		return 1;
	}
	blink_interval = new_interval * US_PER_MS;

	msg_t msg;

	msg.content.value = MSG_CHANGE_INTERVAL;
	if (msg_try_send(&msg, blink_pid) == 0) {
		puts("Receiver queue full.\n");
	}
	else {
		puts("Timer updated\n");
	}
	return 0;
}

#ifdef BTN0_PIN
static void toggle_timer(void *unused) {
	puts("Button pressed, toggling timer\n");
	(void)unused;
	if (blink_pid == 0) {
		puts("Starting timer\n");
		cmd_start_timer(0, NULL);
	}
	else {
		puts("Stopping timer\n");
		cmd_stop_timer(0, NULL);
	}
}
#endif

unsigned short crc16(const unsigned char* data_p, unsigned char length) {
	unsigned char x;
	unsigned short crc = 0xFFFF;

	while (length--) {
		x = crc >> 8 ^ *data_p++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x << 5)) ^ ((unsigned short)x);
	}
	return crc;
}


static void get_board_id(void) {
	cpuid_get(cpuid);
	unsigned short boardid = crc16(cpuid, CPUID_LEN);
	printf("Board ID: %04x\n", boardid);
	printf("Board ID: (unknown)");
}

static int cmd_info(int argc, char **argv) {
	(void)argc;
	(void)argv;

	get_board_id();
	return 0;
}
/*
void *_network_event_loop(void *arg)
{
	(void)arg;

	static msg_t net_queue[NET_QUEUE_SIZE];
	msg_init_queue(net_queue, NET_QUEUE_SIZE);

	msg_t msg, reply;
	reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
	reply.content.value = -ENOTSUP;

	gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(
									 GNRC_NETREG_DEMUX_CTX_ALL,
									 sched_active_pid);

	gnrc_netreg_register(GNRC_NETTYPE_SIXLOWPAN , &me_reg);

	while (1) {
		msg_receive(&msg);
		gnrc_pktsnip_t *pkt = msg.content.ptr;

		switch (msg.type) {
			case GNRC_NETAPI_MSG_TYPE_RCV:
				pkt = msg.content.ptr;
			   // _handle_incoming_pkt(pkt);
				break;
			case GNRC_NETAPI_MSG_TYPE_SND:
				pkt = msg.content.ptr;
			   // _handle_outgoing_pkt(pkt);
				break;
			 case GNRC_NETAPI_MSG_TYPE_SET:
			 case GNRC_NETAPI_MSG_TYPE_GET:
				msg_reply(&msg, &reply);
				break;
			default:
				break;
		}
	}
	return NULL;
}
*/


extern int udp_cmd(int argc, char **argv);
extern int mac_cmd(int argc, char **argv);

const shell_command_t shell_commands[] = {
	{"hello", "prints hello world", hello_world},
	{"toggle", "toggles on-board LED", cmd_toggle_led},
	{"start_timer", "starts periodic blinking", cmd_start_timer},
	{"stop_timer", "stops periodic blinking", cmd_stop_timer},
	{"change_interval", "change blink frequency", cmd_change_interval },
	/*
	{"udp", "send data over UDP and listen on UDP ports", udp_cmd },
	{"mac", "get MAC protocol's internal information", mac_cmd },
	*/
	{"info", "get system info", cmd_info },
	{ NULL, NULL, NULL }
};

int init_tps92661(uint8_t uart_id) {

	uart_half_duplex_params_t tps92661_params = {
		.uart = uart_id,
		.baudrate = 1000,
		.dir = UART_HALF_DUPLEX_DIR_NONE,
	};

	int ret = uart_half_duplex_init(&tps92661_stream, tps92661_buffer, ARRAY_LEN(tps92661_buffer), &tps92661_params);
	if (ret == UART_HALF_DUPLEX_NODEV) {
		puts("Error: invalid UART device given");
		return -1;
	}
	if (ret == UART_HALF_DUPLEX_NOBAUD) {
		puts("Error: given baudrate is not applicable");
		return -1;
	}
	if (ret == UART_HALF_DUPLEX_INTERR) {
		puts("Error: internal error");
		return -1;
	}
	if (ret == UART_HALF_DUPLEX_NOMODE) {
		puts("Error: given mode is not applicable");
		return -1;
	}
	if (ret == UART_HALF_DUPLEX_NOBUFF) {
		puts("Error: invalid buffer given");
		return -1;
	}
	printf("Successfully initialized TPS92661 bus UART_DEV(%i)\n", uart_id);
	
	tps92661_params_t matrix_params = {
		.stream = &tps92661_stream,
		.id = 0,
		.enable_pin = TPS92661_ENABLE_PIN,
		.enable_mode = TPS92661_ENABLE_MODE,
	}; 

	int matrix_result = tps92661_init(&matrix, &matrix_params);


	if (matrix_result == TPS92661_TIMEOUT) {
		puts("Error: timeout pinging TPS92661");
		return -1;
	}
	if (matrix_result == TPS92661_OK) {
		printf("Successfully pinged TPS92661");
		return 0;
	}
	puts("Error: unknown response from TPS92662");
	return -1;
}

int main(void)
{
	init_tps92661(1);

	msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

	blink_pid = 0;
	(void)puts("Started\n");

#ifdef BTN0_PIN
	gpio_init_int(BTN0_PIN, GPIO_IN_PU, GPIO_FALLING, toggle_timer, NULL);
#endif

	char line_buf[SHELL_DEFAULT_BUFSIZE];
	shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

	return 0;
}
