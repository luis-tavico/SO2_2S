#ifndef _SYSCALLS_USAC_H
#define _SYSCALLS_USAC_H

#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/syscalls.h>

// Declaración de las syscalls

// my_encrypt syscall
asmlinkage long sys_my_encrypt(const char __user *ruta_archivo_entrada,
                               const char __user *ruta_archivo_salida,
                               int numero_de_hilos,
                               const char __user *ruta_archivo_clave);

// my_decrypt syscall
asmlinkage long sys_my_decrypt(const char __user *ruta_archivo_entrada,
                               const char __user *ruta_archivo_salida,
                               int numero_de_hilos,
                               const char __user *ruta_archivo_clave);

#endif /* _SYSCALLS_USAC_H */