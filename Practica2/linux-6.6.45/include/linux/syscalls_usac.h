#ifndef _SYSCALLS_USAC_H
#define _SYSCALLS_USAC_H

#include <linux/kernel.h>
#include <linux/syscalls.h>

// Definición de la syscall para my_encrypt
asmlinkage long sys_my_encrypt(const char __user *input_file,
                               const char __user *output_file,
                               int num_threads,
                               const char __user *key_file);

// Definición de la syscall para my_decrypt
asmlinkage long sys_my_decrypt(const char __user *input_file,
                               const char __user *output_file,
                               int num_threads,
                               const char __user *key_file);

#endif



