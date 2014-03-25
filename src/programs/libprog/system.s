bits 64

section .text

global sys_print
sys_print:
  mov rsi, rdi
  xor rdi, rdi
  syscall
  ret

global sys_get_time
  mov rdi, 1
  syscall
  ret

global sys_sleep
sys_sleep:
  mov rsi, rdi
  mov rdi, 2
  syscall
  ret

global sys_in
sys_in:
  mov rax, 0x3
  int 0x21
  ret

global sys_out
sys_out:
  mov rax, 0x4
  int 0x21
  ret

global sys_get_time
sys_get_time:
  mov rdi, 0
  jmp sys_sleep

global sys_thread_exit
sys_thread_exit:
  mov rax, 0x5
  int 0x21
  ret

global sys_pid_kill:
  mov rax, 0x6
  int 0x21
  ret

