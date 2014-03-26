#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdtype.h>

typedef struct {
  uint64_t reserved;
  uint64_t type;
  uint64_t len;
  char message[0xfe8];
} __attribute__((packed)) msg_t;

/**
 * Prints the NULL-terminated string `buffer`.
 */
void sys_print(const char * buffer);

/**
 * Returns the system time in microseconds
 */
uint64_t sys_get_time();

/**
 * Sleeps the current thread for `ticks` microseconds
 */
void sys_sleep(uint64_t micro);

/**
 * Kills the current task.
 */
void sys_exit();

/**
 * Exits the current thread. The task may be left empty if you do this on the
 * last thread, so be mindful.
 */
void sys_thread_exit();

/**
 * Admin only. Notifies the system that this thread should be the system's
 * designated interrupt daemon.
 */
void sys_wants_interurpts();

/**
 * Returns a flag array with IRQ's and other external interrupts masked out.
 */
uint64_t sys_get_interrupts();

/**
 * Create a new socket and get its file descriptor.
 */
uint64_t sys_open();

/**
 * Connect a file descriptor to another task. Usually, you start with PID 0
 * which then tells you about other tasks.
 */
bool sys_connect(uint64_t fd, uint64_t pid);

/**
 * Close a socket with a given file descriptor.
 */
void sys_close(uint64_t fd);

/**
 * Read a packet from a file descriptor. Returns `false` if no packets were in
 * the queue.
 */
bool sys_read(uint64_t fd, msg_t * destPacket);

/**
 * Waits and then returns for the next socket with some data. This may return
 * 0xffffffffffffffff if some other thread called this simultaneously or you are
 * also waiting for interrupts.
 */
uint64_t sys_poll();

#endif
