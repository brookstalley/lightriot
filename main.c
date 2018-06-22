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

#define BLINK_INTERVAL (1000UL * US_PER_MS)
#define MSG_STOP_TIMER (0x0101)

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
            }
        } else {
            printf("blink");
            toggle_led();
            xtimer_usleep(BLINK_INTERVAL);
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
        printf("Invalid parameters");
    } else if (thread_pid == -EOVERFLOW) {
        printf("Error creating timer thread");
    } else {
        blink_pid = thread_pid;
        printf("Timer started with pid %d", blink_pid);
    }
    return 0;
}

static int cmd_stop_timer(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (blink_pid == 0) {
        printf("Timer not running");
        return 0;
    }

    msg_t msg;

    msg.content.value=MSG_STOP_TIMER;
    if (msg_try_send(&msg, blink_pid) == 0) {
        printf("Receiver queue full.\n");
    } else {
        printf("Timer stopping");
    }
    return 0;
}

#ifdef BTN0_PIN
static void toggle_timer(void *unused) {
    (void) unused;
    if (blink_pid !=0) {
        cmd_start_timer(0, NULL);
    } else {
        cmd_stop_timer(0, NULL);
    }
}
#endif

const shell_command_t shell_commands[] = {
    {"hello", "prints hello world", hello_world},
    {"toggle", "toggles on-board LED", cmd_toggle_led},
    {"start_timer", "starts periodic blinking", cmd_start_timer},
    {"stop_timer", "stops periodic blinking", cmd_stop_timer},
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
    (void) puts("Started");

#ifdef BTN0_PIN
    gpio_init_int(BTN0_PIN, GPIO_IN_PU, GPIO_FALLING, toggle_timer, NULL);
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
