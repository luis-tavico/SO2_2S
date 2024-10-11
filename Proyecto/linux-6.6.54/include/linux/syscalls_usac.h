#ifndef _SYSCALLS_USAC_H
#define _SYSCALLS_USAC_H

#include <linux/kernel.h>
#include <linux/syscalls.h>

asmlinkage long obtener_info_memoria(unsigned long *memoria_libre, unsigned long *memoria_usada, unsigned long *memoria_cacheada);
asmlinkage long obtener_info_swap(unsigned long *swap_libre, unsigned long *swap_usada);
asmlinkage long obtener_fallos_pagina(unsigned long *fallos_menores, unsigned long *fallos_mayores);
asmlinkage long obtener_paginas_act_inact(unsigned long *paginas_activas, unsigned long *paginas_inactivas);
asmlinkage long obtener_procesos_memoria(pid_t __user *pids, unsigned long __user *uso_memorias, char __user *nombres_procesos);

#endif