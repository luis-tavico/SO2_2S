# INSTALACION Y MODIFICACION DE KERNEL

# Pasos seguidos

A continuacion se detalla cada uno de los pasos seguidos para la instalacion y modificacion del kernel.

## Actualizar repositorios

Primero es necesario actualizar todos los paquetes instalados, para no tener algun problema de compatibilidad.

```
$ sudo apt update
$ sudo apt upgrade
```

## Instalar dependencias

Luego necesitamos instalar algunos paquetes necesarios para la compilacion.

```
$ sudo apt install build-essential libncurses-dev bison flex libssl-dev libelf-dev fakeroot dwarves
```

## Descargar y descomprimir el kernel

A continuacion, debemos descargar el codigo fuente del kernel desde el sitio web oficial [kernel.org](https://kernel.org/). Usaremos la version ```longterm``` mas reciente del kernel.

![img1](images/img1.png)

Copiamos la direccion del vinculo ```tarball```. Luego usamos este enlace para descargar y descomprimir la fuente del kernel.

```
$ wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.45.tar.xz
$ tar -xf linux-6.6.45.tar.xz
```

## Modificar el kernel

### Llamadas al Sistema

#### Encriptar

Agregaremos una nueva llamada al sistema que requerirá 4 parametros:

* ruta_archivo_entrada: Ruta del archivo a encriptar.
* ruta_archivo_salida: Ruta del archivo encriptado.
* número_de_hilos: Número de hilos con los que se encriptará el archivo.
* ruta_archivo_clave: Ruta del archivo que contiene la clave de encriptación.

El archivo de entrada se divide en n fragmentos (donde n es el número de hilos especiﬁcados). Cada
fragmento se cifra de forma independiente mediante cifrado XOR con la clave proporcionada. Luego, los
fragmentos cifrados se vuelven a escribir en el archivo de salida en el mismo orden.

Para ello primero ingresamos al siguiente directorio: 

```
$ cd linux-6.6.45/kernel/
```

Ahora creamos un nuevo archivo llamado ```my_encrypt.c```.

```
$ touch my_encrypt.c
```

El contenido del archivo sera lo siguiente:

```
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

typedef struct {
    unsigned char *input_data;
    unsigned char *key_data;
    size_t start_idx;
    size_t end_idx;
    size_t key_length;
} thread_args_t;

int xor_cipher_thread_func(void *args) {
    thread_args_t *tdata = (thread_args_t *)args;
    size_t i;

    printk(KERN_INFO "Hilo %s: Procesando cifrado XOR de %zu a %zu\n", current->comm, tdata->start_idx, tdata->end_idx);
    for (i = tdata->start_idx; i < tdata->end_idx; ++i) {
        tdata->input_data[i] ^= tdata->key_data[i % tdata->key_length];
    }
    printk(KERN_INFO "Hilo %s: Cifrado completado\n", current->comm);
    return 0;
}

unsigned char *load_key_file(const char *key_file_path, size_t *key_size) {
    struct file *key_file;
    loff_t offset = 0;
    unsigned char *key_content;

    key_file = filp_open(key_file_path, O_RDONLY, 0);
    if (IS_ERR(key_file)) {
        printk(KERN_ERR "Error al abrir archivo de clave: %s\n", key_file_path);
        return NULL;
    }

    *key_size = i_size_read(file_inode(key_file));
    if (*key_size <= 0) {
        printk(KERN_ERR "Error: archivo de clave con tamaño inválido\n");
        filp_close(key_file, NULL);
        return NULL;
    }

    key_content = kmalloc(*key_size + 1, GFP_KERNEL);
    if (!key_content) {
        printk(KERN_ERR "Error al asignar memoria para la clave\n");
        filp_close(key_file, NULL);
        return NULL;
    }

    kernel_read(key_file, key_content, *key_size, &offset);
    key_content[*key_size] = '\0';
    filp_close(key_file, NULL);
    return key_content;
}

unsigned char *load_input_file(const char *file_path, size_t *file_size) {
    struct file *file;
    loff_t offset = 0;
    unsigned char *file_content;

    file = filp_open(file_path, O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "Error al abrir archivo de entrada: %s\n", file_path);
        return NULL;
    }

    *file_size = i_size_read(file_inode(file));
    if (*file_size <= 0) {
        printk(KERN_ERR "Error: archivo de entrada con tamaño inválido\n");
        filp_close(file, NULL);
        return NULL;
    }

    file_content = kmalloc(*file_size, GFP_KERNEL);
    if (!file_content) {
        printk(KERN_ERR "Error al asignar memoria para el archivo\n");
        filp_close(file, NULL);
        return NULL;
    }

    kernel_read(file, file_content, *file_size, &offset);
    filp_close(file, NULL);
    return file_content;
}

int write_output_file(const char *file_path, unsigned char *data, size_t data_size) {
    struct file *file;
    loff_t offset = 0;
    ssize_t bytes_written;

    file = filp_open(file_path, O_WRONLY | O_CREAT, 0644);
    if (IS_ERR(file)) {
        printk(KERN_ERR "Error al abrir archivo de salida: %s\n", file_path);
        return PTR_ERR(file);
    }

    bytes_written = kernel_write(file, data, data_size, &offset);
    if (bytes_written < 0) {
        printk(KERN_ERR "Error al escribir en archivo de salida\n");
        filp_close(file, NULL);
        return bytes_written;
    }

    filp_close(file, NULL);
    return 0;
}

SYSCALL_DEFINE4(kernel_xor_encrypt, const char __user *, input_file, const char __user *, output_file, int, num_threads, const char __user *, key_file) {
    size_t file_size, key_size;
    unsigned char *file_data, *key_data;
    struct task_struct **threads;
    thread_args_t *thread_args;
    size_t segment_size, start_pos;
    int i, result;

    char kernel_input[256], kernel_output[256], kernel_key[256];
    if (copy_from_user(kernel_input, input_file, sizeof(kernel_input))) return -EFAULT;
    if (copy_from_user(kernel_output, output_file, sizeof(kernel_output))) return -EFAULT;
    if (copy_from_user(kernel_key, key_file, sizeof(kernel_key))) return -EFAULT;

    file_data = load_input_file(kernel_input, &file_size);
    if (!file_data) {
        return -EIO;
    }

    key_data = load_key_file(kernel_key, &key_size);
    if (!key_data) {
        kfree(file_data);
        return -EIO;
    }

    threads = kmalloc_array(num_threads, sizeof(struct task_struct *), GFP_KERNEL);
    thread_args = kmalloc_array(num_threads, sizeof(thread_args_t), GFP_KERNEL);
    if (!threads || !thread_args) {
        kfree(file_data);
        kfree(key_data);
        return -ENOMEM;
    }

    segment_size = file_size / num_threads;
    start_pos = 0;

    for (i = 0; i < num_threads; ++i) {
        thread_args[i].input_data = file_data;
        thread_args[i].key_data = key_data;
        thread_args[i].key_length = key_size;
        thread_args[i].start_idx = start_pos;
        thread_args[i].end_idx = (i == num_threads - 1) ? file_size : (start_pos + segment_size);
        start_pos += segment_size;
        printk(KERN_INFO "Iniciando hilo %d para cifrado desde %zu a %zu\n", i, thread_args[i].start_idx, thread_args[i].end_idx);
        threads[i] = kthread_run(xor_cipher_thread_func, &thread_args[i], "cipher_thread_%d", i);
        if (IS_ERR(threads[i])) {
            printk(KERN_ERR "Error al crear hilo %d\n", i);
            result = PTR_ERR(threads[i]);
            goto cleanup;
        }
    }

    for (i = 0; i < num_threads; ++i) {
        if (threads[i]) {
            kthread_stop(threads[i]);
        }
    }
    result = write_output_file(kernel_output, file_data, file_size);

cleanup:
    kfree(file_data);
    kfree(key_data);
    kfree(threads);
    kfree(thread_args);

    return result;
}
```

#### Desencriptar

Agregaremos una llamada al sistema que requerirá 4 parametros:

* ruta_archivo_entrada: Ruta del archivo a desencriptar.
* ruta_archivo_salida: Ruta del archivo desencriptado.
* número_de_hilos: Número de hilos con los que se desencriptará el archivo.
* ruta_archivo_clave: Ruta del archivo que con�ene la clave de encriptación.

El archivo cifrado se divide en los mismos fragmentos n que durante el cifrado. Cada fragmento se
descifra igualmente de forma independiente mediante el descifrado XOR con la misma clave y número
de hilos utilizados durante el cifrado. Luego, los fragmentos descifrados se vuelven a ensamblar y se
escriben en el archivo de salida en el orden correcto.

Para ello primero ingresamos al siguiente directorio: 

```
$ cd linux-6.6.45/kernel/
```

Ahora creamos un nuevo archivo llamado ```my_decrypt.c```.

```
$ touch my_decrypt.c
```

El contenido del archivo sera lo siguiente:

```
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

typedef struct {
    unsigned char *input_data;
    unsigned char *cipher_key;
    size_t start_pos;
    size_t end_pos;
    size_t key_length;
} thread_task_t;

int xor_decrypt_thread(void *args) {
    thread_task_t *task_data = (thread_task_t *)args;
    size_t i;
    printk(KERN_INFO "Thread %s: Comenzando el proceso de desencriptado XOR desde %zu hasta %zu\n", current->comm, task_data->start_pos, task_data->end_pos);
    
    for (i = task_data->start_pos; i < task_data->end_pos; ++i) {
        task_data->input_data[i] ^= task_data->cipher_key[i % task_data->key_length];
    }
    
    printk(KERN_INFO "Thread %s: Proceso de desencriptado completado\n", current->comm);
    return 0;
}

unsigned char *load_key_from_file(const char *key_path, size_t *key_len) {
    struct file *key_file;
    loff_t offset = 0;
    unsigned char *key_buffer;

    key_file = filp_open(key_path, O_RDONLY, 0);
    if (IS_ERR(key_file)) {
        printk(KERN_ERR "ERROR: No se pudo abrir el archivo de clave: %s\n", key_path);
        return NULL;
    }

    *key_len = i_size_read(file_inode(key_file));
    if (*key_len <= 0) {
        printk(KERN_ERR "ERROR: Tamaño del archivo de clave no válido\n");
        filp_close(key_file, NULL);
        return NULL;
    }

    key_buffer = kmalloc(*key_len + 1, GFP_KERNEL);
    if (!key_buffer) {
        printk(KERN_ERR "ERROR: No se pudo asignar memoria para la clave\n");
        filp_close(key_file, NULL);
        return NULL;
    }

    kernel_read(key_file, key_buffer, *key_len, &offset);
    key_buffer[*key_len] = '\0';  
    filp_close(key_file, NULL);
    return key_buffer;
}

unsigned char *load_file_to_memory(const char *file_path, size_t *file_len) {
    struct file *input_file;
    loff_t offset = 0;
    unsigned char *buffer;

    input_file = filp_open(file_path, O_RDONLY, 0);
    if (IS_ERR(input_file)) {
        printk(KERN_ERR "ERROR: No se pudo abrir el archivo: %s\n", file_path);
        return NULL;
    }

    *file_len = i_size_read(file_inode(input_file));
    if (*file_len <= 0) {
        printk(KERN_ERR "ERROR: Tamaño del archivo no válido\n");
        filp_close(input_file, NULL);
        return NULL;
    }

    buffer = kmalloc(*file_len, GFP_KERNEL);
    if (!buffer) {
        printk(KERN_ERR "ERROR: No se pudo asignar memoria para el archivo\n");
        filp_close(input_file, NULL);
        return NULL;
    }

    kernel_read(input_file, buffer, *file_len, &offset);
    filp_close(input_file, NULL);
    return buffer;
}

int save_data_to_file(const char *output_path, unsigned char *output_data, size_t data_size) {
    struct file *output_file;
    loff_t offset = 0;
    ssize_t bytes_written;

    output_file = filp_open(output_path, O_WRONLY | O_CREAT, 0644);
    if (IS_ERR(output_file)) {
        printk(KERN_ERR "ERROR: No se pudo abrir el archivo de salida: %s\n", output_path);
        return PTR_ERR(output_file);
    }

    bytes_written = kernel_write(output_file, output_data, data_size, &offset);
    if (bytes_written < 0) {
        printk(KERN_ERR "ERROR: Fallo al escribir en el archivo de salida\n");
        filp_close(output_file, NULL);
        return bytes_written;
    }

    filp_close(output_file, NULL);
    return 0;
}

SYSCALL_DEFINE4(decrypt_xor_syscall, const char __user *, src_file, const char __user *, dest_file, int, num_of_threads, const char __user *, key_path) {
    size_t file_length, key_length;
    unsigned char *input_data, *key_data;
    struct task_struct **thread_pool;
    thread_task_t *tasks;
    size_t segment_size, start_offset;
    int i, ret;

    char kernel_src_file[256], kernel_dest_file[256], kernel_key_file[256];
    if (copy_from_user(kernel_src_file, src_file, sizeof(kernel_src_file))) return -EFAULT;
    if (copy_from_user(kernel_dest_file, dest_file, sizeof(kernel_dest_file))) return -EFAULT;
    if (copy_from_user(kernel_key_file, key_path, sizeof(kernel_key_file))) return -EFAULT;

    input_data = load_file_to_memory(kernel_src_file, &file_length);
    if (!input_data) {
        return -EIO;
    }

    key_data = load_key_from_file(kernel_key_file, &key_length);
    if (!key_data) {
        kfree(input_data);
        return -EIO;
    }

    thread_pool = kmalloc_array(num_of_threads, sizeof(struct task_struct *), GFP_KERNEL);
    tasks = kmalloc_array(num_of_threads, sizeof(thread_task_t), GFP_KERNEL);
    if (!thread_pool || !tasks) {
        kfree(input_data);
        kfree(key_data);
        return -ENOMEM;
    }

    segment_size = file_length / num_of_threads;
    start_offset = 0;

    for (i = 0; i < num_of_threads; ++i) {
        tasks[i].input_data = input_data;
        tasks[i].cipher_key = key_data;
        tasks[i].key_length = key_length;
        tasks[i].start_pos = start_offset;
        tasks[i].end_pos = (i == num_of_threads - 1) ? file_length : (start_offset + segment_size);
        start_offset += segment_size;

        printk(KERN_INFO "Lanzando hilo %d para desencriptar desde byte %zu a %zu\n", i, tasks[i].start_pos, tasks[i].end_pos);

        thread_pool[i] = kthread_run(xor_decrypt_thread, &tasks[i], "thread_xor_%d", i);
        if (IS_ERR(thread_pool[i])) {
            printk(KERN_ERR "ERROR: No se pudo iniciar el hilo %d\n", i);
            ret = PTR_ERR(thread_pool[i]);
            goto cleanup;
        }
    }

    for (i = 0; i < num_of_threads; ++i) {
        if (thread_pool[i]) {
            kthread_stop(thread_pool[i]);
        }
    }

    ret = save_data_to_file(kernel_dest_file, input_data, file_length);

cleanup:
    kfree(input_data);
    kfree(key_data);
    kfree(thread_pool);
    kfree(tasks);

    return ret;
}
```

### Incluir archivos objeto

Ahora debemos generar los archivos objeto, para ello ingresamos al siguiente directorio:

```
$ cd linux-6.6.45/kernel/
```

Ahora buscamos el archivo ```Makefile``` y agregamos la siguientes lineas de codigo:

```
obj-y += my_encrypt.o
obj-y += my_decrypt.o
```

### Registrar Llamadas al Sistema

Registraremos las llamadas al sistema realizadas anteriormente en la tabla de llamadas del sistema, para ello primero ingresamos al siguiente directorio: 

```
$ cd linux-6.6.45/arch/x86/entry/syscalls/
```

Luego buscamos el archivo ```syscall_64.tbl``` y agregamos la siguientes lineas de codigo al final del archivo.

```
548 common  my_encrypt  sys_my_encrypt
549 common  my_decrypt  sys_my_decrypt
```

Ahora agregamos las declaraciones de las nuevas llamadas al sistema, entonces ingresamos al siguiente directorio: 

```
$ cd linux-6.6.45/include/linux/
```

Luego crearemos un archivo el cual nombraremos ```syscalls_usac.h``` y agregamos la siguientes lineas de codigo al archivo.

```
#ifndef _SYSCALLS_USAC_H
#define _SYSCALLS_USAC_H

#include <linux/kernel.h>
#include <linux/syscalls.h>

asmlinkage long sys_my_encrypt(const char __user *input_file, const char __user *output_file, int num_threads, const char __user *key_file);
asmlinkage long sys_my_decrypt(const char __user *input_file, const char __user *output_file, int num_threads, const char __user *key_file);

#endif
```

## Crear achivos de prueba

Para verificar que las llamadas al sistema funcionen bien, se proceden a realizar archivos de prueba que ejecuten las llamadas al sistema creadas, para ello primero debemos crear dos archivos.

El primer archivo lo nombraremos ```encrypt_app``` y para crerlo hacemos lo siguiente.

```
$ touch encrypt_app.c
```

Luego agregamos el siguiente bloque de codigo:

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

#define MY_ENCRYPT_SYSCALL 548

void mostrar_uso() {
    fprintf(stderr, "Ingresa: -p <archivo_entrada> -o <archivo_salida> -t <num_hilos> -k <archivo_clave>\nIngresa: 'salir' si deseas terminar el programa.\n");
    printf("\n");
}

int main() {
    char comando[256];
    const char *archivo_entrada = NULL;
    const char *archivo_salida = NULL;
    const char *archivo_clave = NULL;
    int num_hilos = 0;
    int error_parametros = 0;

    // Ciclo que sigue pidiendo comandos hasta que se ingrese "salir"
    while (1) {
        // Solicitar entrada del usuario
        printf("> ");
        fgets(comando, sizeof(comando), stdin);

        // Eliminar el salto de línea al final de la entrada
        comando[strcspn(comando, "\n")] = 0;

        // Si el usuario ingresa "salir", finalizar el programa
        if (strcmp(comando, "salir") == 0) {
            printf("Terminando el programa.\n");
            break;
        }

        // Separar los argumentos del comando ingresado
        char *argumentos[10]; // Capacidad para hasta 10 argumentos
        int num_argumentos = 0;
        char *token = strtok(comando, " ");
        while (token != NULL && num_argumentos < 10) {
            argumentos[num_argumentos++] = token;
            token = strtok(NULL, " ");
        }

        // Inicializar valores
        archivo_entrada = NULL;
        archivo_salida = NULL;
        archivo_clave = NULL;
        num_hilos = 0;
        error_parametros = 0;

        // Procesar los argumentos
        for (int i = 0; i < num_argumentos; i++) {
            if (strcmp(argumentos[i], "-p") == 0 && i + 1 < num_argumentos) {
                archivo_entrada = argumentos[++i];
            } else if (strcmp(argumentos[i], "-o") == 0 && i + 1 < num_argumentos) {
                archivo_salida = argumentos[++i];
            } else if (strcmp(argumentos[i], "-t") == 0 && i + 1 < num_argumentos) {
                num_hilos = atoi(argumentos[++i]);
                if (num_hilos <= 0) {
                    fprintf(stderr, "Error: El parámetro -t para el número de hilos es inválido o falta (debe ser mayor que 0)\n");
                    error_parametros = 1;
                }
            } else if (strcmp(argumentos[i], "-k") == 0 && i + 1 < num_argumentos) {
                archivo_clave = argumentos[++i];
            } else {
                mostrar_uso();
                error_parametros = 1;
                break;
            }
        }

        // Verificar si faltan parámetros obligatorios
        if (!archivo_entrada) {
            fprintf(stderr, "Error: Falta el parámetro -p para el archivo de entrada\n");
            error_parametros = 1;
        }
        if (!archivo_salida) {
            fprintf(stderr, "Error: Falta el parámetro -o para el archivo de salida\n");
            error_parametros = 1;
        }
        if (!archivo_clave) {
            fprintf(stderr, "Error: Falta el parámetro -k para el archivo de clave\n");
            error_parametros = 1;
        }

        // Si hay errores en los parámetros, mostrar el uso y no ejecutar
        if (error_parametros) {
            mostrar_uso();
            continue;
        }

        printf("Iniciando encriptación...\n");
        printf("Archivo de entrada: %s\n", archivo_entrada);
        printf("Archivo de salida: %s\n", archivo_salida);
        printf("Número de hilos: %d\n", num_hilos);
        printf("Archivo de clave: %s\n", archivo_clave);

        // Realizar la llamada al sistema `my_encrypt`
        long resultado = syscall(MY_ENCRYPT_SYSCALL, archivo_entrada, archivo_salida, num_hilos, archivo_clave);

        if (resultado != 0) {
            perror("Error: Fallo en my_encrypt");
            printf("\n");
        } else {
            // Si la encriptación fue exitosa
            printf("Encriptación completada exitosamente. Archivo de salida: %s\n", archivo_salida);
            printf("\n");
        }
    }

    return 0;
}
```

El segundo archivo lo nombraremos ```decrypt_app``` y para crerlo hacemos lo siguiente.

```
$ touch decrypt_app.c
```

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

#define MY_DECRYPT_SYSCALL 549

void mostrar_uso() {
    fprintf(stderr, "Ingresa: -p <archivo_entrada> -o <archivo_salida> -t <num_hilos> -k <archivo_clave>\nIngresa: 'salir' si deseas terminar el programa.\n");
    printf("\n");
}

int main() {
    char comando[256];
    const char *archivo_entrada = NULL;
    const char *archivo_salida = NULL;
    const char *archivo_clave = NULL;
    int num_hilos = 0;
    int error_parametros = 0;

    // Ejecutar el ciclo hasta que el usuario ingrese 'salir'
    while (1) {
        // Solicitar comando al usuario
        printf("> ");
        fgets(comando, sizeof(comando), stdin);

        // Quitar salto de línea
        comando[strcspn(comando, "\n")] = 0;

        // Verificar si el usuario quiere salir
        if (strcmp(comando, "salir") == 0) {
            printf("Saliendo del programa.\n");
            break;
        }

        // Separar la entrada en argumentos
        char *argumentos[10]; // Soporta hasta 10 argumentos
        int num_argumentos = 0;
        char *token = strtok(comando, " ");
        while (token != NULL && num_argumentos < 10) {
            argumentos[num_argumentos++] = token;
            token = strtok(NULL, " ");
        }

        // Reiniciar valores por cada iteración
        archivo_entrada = NULL;
        archivo_salida = NULL;
        archivo_clave = NULL;
        num_hilos = 0;
        error_parametros = 0;

        // Procesar los argumentos ingresados
        for (int i = 0; i < num_argumentos; i++) {
            if (strcmp(argumentos[i], "-p") == 0 && i + 1 < num_argumentos) {
                archivo_entrada = argumentos[++i];
            } else if (strcmp(argumentos[i], "-o") == 0 && i + 1 < num_argumentos) {
                archivo_salida = argumentos[++i];
            } else if (strcmp(argumentos[i], "-t") == 0 && i + 1 < num_argumentos) {
                num_hilos = atoi(argumentos[++i]);
            } else if (strcmp(argumentos[i], "-k") == 0 && i + 1 < num_argumentos) {
                archivo_clave = argumentos[++i];
            } else {
                mostrar_uso();
                continue;
            }
        }

        // Verificar parámetros obligatorios
        if (!archivo_entrada) {
            fprintf(stderr, "Error: Falta el parámetro -p para especificar el archivo de entrada\n");
            error_parametros = 1;
        }
        if (!archivo_salida) {
            fprintf(stderr, "Error: Falta el parámetro -o para el archivo de salida\n");
            error_parametros = 1;
        }
        if (num_hilos <= 0) {
            fprintf(stderr, "Error: El número de hilos especificado con -t es inválido o falta (debe ser mayor a 0)\n");
            error_parametros = 1;
        }
        if (!archivo_clave) {
            fprintf(stderr, "Error: Falta el archivo de clave, use -k para especificarlo\n");
            error_parametros = 1;
        }

        // Si algún parámetro falta, no ejecutar
        if (error_parametros) {
            mostrar_uso();
            continue;
        }

        printf("Iniciando proceso de desencriptación...\n");
        printf("Archivo de entrada: %s\n", archivo_entrada);
        printf("Archivo de salida: %s\n", archivo_salida);
        printf("Hilos utilizados: %d\n", num_hilos);
        printf("Archivo clave: %s\n", archivo_clave);

        // Llamada al sistema `my_decrypt`
        long resultado = syscall(MY_DECRYPT_SYSCALL, archivo_entrada, archivo_salida, num_hilos, archivo_clave);
        
        if (resultado != 0) {
            perror("Error al ejecutar my_decrypt");
            printf("\n");
        } else {
            printf("Desencriptación completada exitosamente\n");
            printf("\n");
        }
    }

    return 0;
}
```

Ahora debemos compilar los archivos creados, entonces ejecutamos el siguiente comando:

```
$ gcc -o encrypt_app encrypt_app.c
$ gcc -o decrypt_app decrypt_app.c
```

Despues de compilar podemos ejecutar el programa para probar las nuevas llamadas al sistema con el siguiente comando:

En caso de encriptar:

```
$ ./encrypt_app
```

En caso de desencriptar:

```
$ ./decrypt_app
```

## Configurar el kernel

Primero ingrasamos al directorio del codigo fuente.

```
$ cd linux-6.6.45
```

La configuracion del kernel se debe especificar en un archivo .config. Para no escribir este desde 0 vamos a copiar el archivo de configuracion del Linux actualmente instalado.

```
$ cp -v /boot/config-$(uname -r) .config
```

Sin embargo, este esta lleno de modulos y drivers que no necesitamos que pueden aumentar el tiempo de compilación. Por lo que utilizamos el comando ```localmodconfig``` que analiza los módulos del kernel cargados del sistema y modifica el archivo .config de modo que solo estos modulos se incluyan en la compilacion.

```
$ make localmodconfig
```

Luego tenemos que modificar el .config, ya que al copiar nuestro .config se incluyeron nuestras llaves privadas, por lo que tendremos que reliminarlas del .config.

```
$ scripts/config --disable SYSTEM_TRUSTED_KEYS
$ scripts/config --disable SYSTEM_REVOCATION_KEYS
$ scripts/config --set-str CONFIG_SYSTEM_TRUSTED_KEYS ""
$ scripts/config --set-str CONFIG_SYSTEM_REVOCATION_KEYS ""
```

## Compilar el kernel

Ahora es el momento de compilar el kernel. Para esto simplemente ejecutamos el comando:

```
$ fakeroot make -j4
```

Donde ```-j4``` es la cantidad de nucleos que se deseamos utilizar para compilar el kernel.

Utilizar ```fakeroot``` es necesario por que nos permite ejecutar el comando ```make``` en  un  entorno  donde  parece  que  se  tiene  permisos  de superusuario  para  la  manipulacion  de  ficheros.  Es necesario  para  permitir a este comando crear archivos (tar, ar, .deb etc.) con ficheros con permisos/propietarios de superusuario.

## Instalar el kernel

La instalacion se divide en dos partes: instalar los modulos del kernel e instalar el kernel mismo.

Primero se instalan los modulos del kernel ejecutando:

```
$ sudo make modules_install
```

Luego instalamos el kernel ejecutando:

```
$ sudo make install
```

Despues de eso, reiniciamos la computadora para que se complete la instalacion.

```
$ sudo reboot
```

Ahora hemos terminado de instalar el kernel con los cambios necesarios.

# Errores ocurridos

A continuacion se detalla cada uno de los errores que ocurrieron durante la instalacion y modificacion del kernel.

## Syscalls no fueron correctamente registradas

Este error ocurrio debido a que el sistema se quedo sin memoria o recursos y por lo tanto el proceso de compilacion fallo.

Este error indica que el compilador no encuentra referencias a las funciones del sistema (```my_encrypt``` y ```my_decrypt```) en el proceso de enlace final del kernel.

![img2](images/img2.png)

Para solucionar este error se corroboro que los archivos .o en las syscalls estén correctamente definidos en el Makefile.

![img2-1](images/img2-1.png)

## Falta de espacio

Este error ocurrio debido a la falta de espacio en el dispositivo y por lo tanto el proceso de compilacion fallo.

![img3](images/img3.png)

La causa de este error se debe a que anteriormente se realizaron varias compilaciones y esto provoco que archivos temporales ocuparan demasiado espacio.

Para solucionar este error se ejecuto el siguiente comando:

```
make clean
```

Este comando se encargar de limpiar el espacio de trabajo usado.

## Funcion no compila

Para solucionar este error se debe ingresar al siguiente directorio:

```
$ cd linux-6.6.45/kernel/
```

Luego debemos agregar la llamada a las funciones en el archivo ```Makefile```, esto forzara al kernel a compilar las funciones nuevas.