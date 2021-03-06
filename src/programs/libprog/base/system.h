#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdtype.h>

typedef struct {
  uint64_t reserved;
  uint64_t type;
  uint64_t len;
  char message[0xfe8];
} __attribute__((packed)) msg_t;

typedef struct {
  uint64_t taskId;
  uint64_t threadId;
  uint64_t address;
  uint64_t flags;
} pgf_t;

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
void sys_wants_interrupts();

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
 * Write a packet to a file descriptor. Returns `false` if the buffer is full.
 */
uint64_t sys_write(uint64_t fd, const void * data, uint64_t len);

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

/**
 * Gets the remote PID for a socket. Returns (uint64_t)-1 on error or if there
 * is no other end.
 */
uint64_t sys_remote_pid(uint64_t fd);

/**
 * See sys_remote_pid().
 */
uint64_t sys_remote_uid(uint64_t fd);

/**
 * Input from an I/O pin. The `size` must be 1 or 2.  This requires root.
 */
uint64_t sys_in(uint64_t pin, uint64_t size);

/**
 * Output to an I/O pin. This requires root.
 */
uint64_t sys_out(uint64_t pin, uint64_t val, uint64_t size);

/**
 * Set the print color.
 */
void sys_color(uint8_t color);

/**
 * Launch a new task and return an opened socket to it.
 */
uint64_t sys_fork(uint64_t code);

/**
 * Returns the number of pages allocated by the page allocator.
 */
uint64_t sys_mem_usage();

/**
 * Kill a task with a given PID. Returns true if the task was found and the
 * permissions allowed it to be killed. A return value of true does not mean the
 * task shutdown has finished; it simply means it has begun.
 */
bool sys_kill(uint64_t pid);

/**
 * Returns an address in the physical address space to a new physical page of
 * memory.
 */
uint64_t sys_alloc_page();

/**
 * Returns an address in the physical address space to a buffer of at least
 * `pages` 4KB pages. The pointer will be aligned to 2^n bytes, where n is the
 * first integer such that 2^n >= pages. Thus, if you want an aligned buffer,
 * you better pass in a power of 2.
 */
uint64_t sys_alloc_pci(uint64_t pages);

/**
 * Free a page of physical memory allocated with sys_alloc_page().
 */
void sys_free_page(uint64_t addr);

/**
 * Free a chunk of physical memory allocated with sys_alloc_pci().
 */
void sys_free_pci(uint64_t addr, uint64_t pages);

/**
 * Map a virtual page to a physical page in a task's page table.
 * @return false if the task does not exist or the system is out of memory.
 */
bool sys_vmmap(uint64_t fd, uint64_t vpage, uint64_t entry);

/**
 * Unmap a virtual page in a task's page table.
 * @return false if the task does not exist.
 */
bool sys_vmunmap(uint64_t fd, uint64_t vpage);

/**
 * Notify every CPU running a task that it should flush its TLB cache.
 */
bool sys_invlpg(uint64_t fd);

/**
 * Launch a thread which will automatically exit when you return. The stack on
 * this function call will be 16-byte aligned like the Mac ABI says.
 */
void sys_launch_thread(void (*)(void *), void * arg);

/**
 * Return the current thread ID.
 */
uint64_t sys_thread_id();

/**
 * Stop a thread from sleeping.
 */
void sys_unsleep(uint64_t threadId);

/**
 * Returns the current UID.
 */
uint64_t sys_self_uid();

/**
 * Returns the current PID.
 */
uint64_t sys_self_pid();

/**
 * Returns the physical mapping for a virtual page in this task.
 */
uint64_t sys_vmread(uint64_t page);

/**
 * Request that THIS task become the system pager. This should really only be
 * called from UID 0.
 */
void sys_become_pager();

/**
 * Get the next page fault from the queue, or return `false` on an empty queue.
 */
bool sys_get_fault(pgf_t * fault);

/**
 * Map a page of physical memory into our task's virtual address space.
 */
bool sys_self_vmmap(uint64_t vpage, uint64_t entry);

/**
 * Undo something done with sys_self_vmmap().
 */
void sys_self_vmunmap(uint64_t vpage);

/**
 * Call this whenever we finish updating our page tables.
 */
void sys_self_invlpg();

/**
 * Shift the fault queue. You must be the system pager.
 */
void sys_shift_fault();

/**
 * Abort the current task. Like sys_exit() except with the ABORT kill reason.
 */
void sys_abort();

/**
 * Cause a memory fault in a task which kills it.
 * @return true if the task was found and killed, false otherwise.
 */
bool sys_mem_fault(uint64_t pid);

/**
 * Wakeup a PID and Thread ID. This requires that you are the system pager!
 * After you call this method, you must not reference this TID anymore, because
 * the thread may choose to exit the *instant* it begins executing.
 */
void sys_wake_thread(uint64_t fd, uint64_t tid);

/**
 * Allows the system pager to unmap a list of pages from a remote task's address
 * space.
 */
bool sys_batch_vmunmap(uint64_t fd, uint64_t start, uint64_t count);

/**
 * Allocate `count` pages. Output their addresses to a stack variable.
 */
void sys_batch_alloc(uint64_t * output, uint64_t count);

/**
 * Map a batch of addresses into a remote task's address space. The first item
 * in `buf` is the starting virtual page index.
 */
bool sys_batch_vmmap(uint64_t fd, uint64_t * buf, uint64_t count);

void sys_clear_unsleep();

#endif
