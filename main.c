#include <stdio.h>
#include <time.h>
#include <string.h>

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "xtimer.h"
#include "msg.h"

#include "board.h"
#include "periph/gpio.h"
#include "led.h"

#ifdef MODULE_NETIF
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#endif

static kernel_pid_t blink_pid;
static char blink_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];

#define RCV_QUEUE_SIZE (8U)
static msg_t rcv_queue[RCV_QUEUE_SIZE];

#define BLINK_INTERVAL_DEFAULT (1000UL * US_PER_MS)
static unsigned long blink_interval = BLINK_INTERVAL_DEFAULT;

#define MSG_STOP_TIMER (0x0101)
#define MSG_CHANGE_INTERVAL (0x0102)

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
    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);

    while(timer_run) {
        if (msg_try_receive(&msg) == 1) {
            if (msg.content.value == MSG_STOP_TIMER) {
                timer_run = false;
            } else if (msg.content.value == MSG_CHANGE_INTERVAL) {
                // Nothing for now because we'll automatically pick it up
                puts("Timer interval changed\n");
            }
        } else {
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
        (void) puts("Invalid parameters\n");
    } else if (thread_pid == -EOVERFLOW) {
        puts("Error creating timer thread\n");
    } else {
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

    msg.content.value=MSG_STOP_TIMER;
    if (msg_try_send(&msg, blink_pid) == 0) {
        puts("Receiver queue full.\n");
    } else {
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
    if (new_interval <=0) {
        puts("Must specify a positive integer for milliseconds");
        return 1;
    }
    blink_interval = new_interval * US_PER_MS;

    msg_t msg;

    msg.content.value=MSG_CHANGE_INTERVAL;
    if (msg_try_send(&msg, blink_pid) == 0) {
        puts("Receiver queue full.\n");
    } else {
        puts("Timer updated\n");
    }
    return 0;
}

#ifdef BTN0_PIN
static void toggle_timer(void *unused) {
    puts("Button pressed, toggling timer\n");
    (void) unused;
    if (blink_pid ==0) {
        puts("Starting timer\n");
        cmd_start_timer(0, NULL);
    } else {
        puts("Stopping timer\n");
        cmd_stop_timer(0, NULL);
    }
}
#endif

const shell_command_t shell_commands[] = {
    {"hello", "prints hello world", hello_world},
    {"toggle", "toggles on-board LED", cmd_toggle_led},
    {"start_timer", "starts periodic blinking", cmd_start_timer},
    {"stop_timer", "stops periodic blinking", cmd_stop_timer},
    {"change_interval", "change blink frequency", cmd_change_interval },
    { NULL, NULL, NULL }
};

int main(void)
{
#ifdef MODULE_NETIF
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                          gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
#endif

    blink_pid = 0;
    (void) puts("Started\n");

#ifdef BTN0_PIN
    gpio_init_int(BTN0_PIN, GPIO_IN_PU, GPIO_FALLING, toggle_timer, NULL);
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
