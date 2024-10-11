#ifndef _LINUX_SYSCALLS_USAC_H
#define _LINUX_SYSCALLS_USAC_H

#include <linux/types.h>  // Incluye los tipos de datos básicos como size_t y ssize_t
#include <linux/syscalls.h>  // Incluye la infraestructura de llamadas al sistema

asmlinkage long sys_my_decrypt(int input_fd, int output_fd, int threads, int key_fd);
asmlinkage long sys_my_encrypt(int input_fd, int output_fd, int threads, int key_fd);

asmlinkage long get_memory_info(unsigned long *mem_free, unsigned long *mem_used, unsigned long *mem_cached);
asmlinkage long get_swap_info(unsigned long *swap_free, unsigned long *swap_used);
asmlinkage long get_page_faults(unsigned long *minor_faults, unsigned long *major_faults);
asmlinkage long get_act_inact_pages(unsigned long *active_pages, unsigned long *inactive_pages);
asmlinkage long get_memory_processes(pid_t __user *pids, unsigned long __user *mem_usage, char __user *names);

#endif  /* _LINUX_SYSCALLS_USAC_H */
