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
$ wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.54.tar.xz
$ tar -xf linux-6.6.54.tar.xz
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
#include <linux/mutex.h> 

static DEFINE_MUTEX(sync_lock);

typedef struct {
    unsigned char *input_data;
    unsigned char *cipher_key;
    size_t range_start;
    size_t range_end;
    size_t key_length;
} worker_args_t;

int xor_encrypt_worker(void *params) {
    worker_args_t *args = (worker_args_t *)params;
    size_t idx;
    int result;

    result = mutex_lock_interruptible(&sync_lock);
    if (result) {
        return -EINTR;
    }

    for (idx = args->range_start; idx < args->range_end; ++idx) {
        if (kthread_should_stop()) {
            mutex_unlock(&sync_lock);
            return -EINTR;
        }
        args->input_data[idx] ^= args->cipher_key[idx % args->key_length];
    }
    mutex_unlock(&sync_lock);
    return 0;
}

unsigned char *load_key_from_file(const char *key_filepath, size_t *key_len) {
    struct file *key_file;
    loff_t offset = 0;
    unsigned char *key_buffer;

    key_file = filp_open(key_filepath, O_RDONLY, 0);
    if (IS_ERR(key_file)) {
        return NULL;
    }

    *key_len = i_size_read(file_inode(key_file));
    if (*key_len <= 0) {
        filp_close(key_file, NULL);
        return NULL;
    }

    key_buffer = kmalloc(*key_len + 1, GFP_KERNEL);
    if (!key_buffer) {
        filp_close(key_file, NULL);
        return NULL;
    }

    kernel_read(key_file, key_buffer, *key_len, &offset);
    key_buffer[*key_len] = '\0';  
    filp_close(key_file, NULL);
    return key_buffer;
}

unsigned char *read_data_from_file(const char *filename, size_t *data_size) {
    struct file *input_file;
    loff_t offset = 0;
    unsigned char *buffer;

    input_file = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(input_file)) {
        return NULL;
    }

    *data_size = i_size_read(file_inode(input_file));
    if (*data_size <= 0) {
        filp_close(input_file, NULL);
        return NULL;
    }

    buffer = kmalloc(*data_size, GFP_KERNEL);
    if (!buffer) {
        filp_close(input_file, NULL);
        return NULL;
    }

    kernel_read(input_file, buffer, *data_size, &offset);
    filp_close(input_file, NULL);
    return buffer;
}

int save_data_to_file(const char *filename, unsigned char *output_data, size_t data_length) {
    struct file *output_file;
    loff_t offset = 0;
    ssize_t written;

    output_file = filp_open(filename, O_WRONLY | O_CREAT, 0644);
    if (IS_ERR(output_file)) {
        printk(KERN_ERR "ERROR: No se pudo abrir el archivo de salida: %s\n", filename);
        return PTR_ERR(output_file);
    }

    written = kernel_write(output_file, output_data, data_length, &offset);
    if (written < 0) {
        printk(KERN_ERR "ERROR: Error al escribir los datos\n");
        filp_close(output_file, NULL);
        return written;
    }

    filp_close(output_file, NULL);
    return 0;
}

SYSCALL_DEFINE4(xor_encrypt_syscall, const char __user *, src_file, const char __user *, dest_file, int, num_threads, const char __user *, key_filepath) {
    size_t data_len, key_len;
    unsigned char *file_data, *key_data;
    struct task_struct **workers;
    worker_args_t *worker_args;
    size_t segment_size, offset;
    int i, result = 0;

    char kernel_src_file[256], kernel_dest_file[256], kernel_key_file[256];
    if (copy_from_user(kernel_src_file, src_file, sizeof(kernel_src_file))) return -EFAULT;
    if (copy_from_user(kernel_dest_file, dest_file, sizeof(kernel_dest_file))) return -EFAULT;
    if (copy_from_user(kernel_key_file, key_filepath, sizeof(kernel_key_file))) return -EFAULT;

    file_data = read_data_from_file(kernel_src_file, &data_len);
    if (!file_data) {
        return -EIO;
    }

    key_data = load_key_from_file(kernel_key_file, &key_len);
    if (!key_data) {
        kfree(file_data);
        return -EIO;
    }

    workers = kmalloc_array(num_threads, sizeof(struct task_struct *), GFP_KERNEL);
    worker_args = kmalloc_array(num_threads, sizeof(worker_args_t), GFP_KERNEL);
    if (!workers || !worker_args) {
        kfree(file_data);
        kfree(key_data);
        return -ENOMEM;
    }
    segment_size = data_len / num_threads;
    offset = 0;

    for (i = 0; i < num_threads; ++i) {
        worker_args[i].input_data = file_data;
        worker_args[i].cipher_key = key_data;
        worker_args[i].key_length = key_len;
        worker_args[i].range_start = offset;
        worker_args[i].range_end = (i == num_threads - 1) ? data_len : (offset + segment_size);
        offset += segment_size;
        workers[i] = kthread_run(xor_encrypt_worker, &worker_args[i], "xor_thread_worker_%d", i);
        if (IS_ERR(workers[i])) {
            printk(KERN_ERR "ERROR: Fallo al crear el hilo %d\n", i);
            result = PTR_ERR(workers[i]);
            goto cleanup;
        }
    }

    for (i = 0; i < num_threads; ++i) {
        if (workers[i]) {
            int worker_result = kthread_stop(workers[i]);
            if (worker_result != 0 && worker_result != -EINTR) {
                printk(KERN_ERR "ERROR: El hilo %d finalizó con error %d\n", i, worker_result);
            } else {
                printk(KERN_INFO "Hilo %d finalizó correctamente\n", i);
            }
        }
    }

    result = save_data_to_file(kernel_dest_file, file_data, data_len);

cleanup:
    kfree(file_data);
    kfree(key_data);
    kfree(workers);
    kfree(worker_args);

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
#include <linux/mutex.h>

static DEFINE_MUTEX(decrypt_mutex);

typedef struct {
    unsigned char *buffer;
    unsigned char *key;
    size_t start_offset;
    size_t end_offset;
    size_t key_length;
} decrypt_thread_args_t;

int xor_decrypt_worker(void *arg) {
    decrypt_thread_args_t *args = (decrypt_thread_args_t *)arg;
    size_t i;
    int lock_status;

    lock_status = mutex_lock_interruptible(&decrypt_mutex);
    if (lock_status) {
        return -EINTR;
    }

    for (i = args->start_offset; i < args->end_offset; ++i) {
        if (kthread_should_stop()) {
            mutex_unlock(&decrypt_mutex);
            return -EINTR;
        }
        args->buffer[i] ^= args->key[i % args->key_length];
    }
    mutex_unlock(&decrypt_mutex);
    return 0;
}

unsigned char *load_key(const char *key_path, size_t *key_size) {
    struct file *key_file;
    loff_t file_pos = 0;
    unsigned char *key_buffer;

    key_file = filp_open(key_path, O_RDONLY, 0);
    if (IS_ERR(key_file)) {
        return NULL;
    }

    *key_size = i_size_read(file_inode(key_file));
    if (*key_size <= 0) {
        filp_close(key_file, NULL);
        return NULL;
    }

    key_buffer = kmalloc(*key_size + 1, GFP_KERNEL);
    if (!key_buffer) {
        filp_close(key_file, NULL);
        return NULL;
    }

    kernel_read(key_file, key_buffer, *key_size, &file_pos);
    key_buffer[*key_size] = '\0';
    filp_close(key_file, NULL);
    return key_buffer;
}

unsigned char *load_data(const char *input_path, size_t *data_size) {
    struct file *input_file;
    loff_t file_pos = 0;
    unsigned char *data_buffer;

    input_file = filp_open(input_path, O_RDONLY, 0);
    if (IS_ERR(input_file)) {
        return NULL;
    }

    *data_size = i_size_read(file_inode(input_file));
    if (*data_size <= 0) {
        filp_close(input_file, NULL);
        return NULL;
    }

    data_buffer = kmalloc(*data_size, GFP_KERNEL);
    if (!data_buffer) {
        filp_close(input_file, NULL);
        return NULL;
    }

    kernel_read(input_file, data_buffer, *data_size, &file_pos);
    filp_close(input_file, NULL);
    return data_buffer;
}

int save_data(const char *output_path, unsigned char *data, size_t data_size) {
    struct file *output_file;
    loff_t file_pos = 0;
    ssize_t bytes_written;

    output_file = filp_open(output_path, O_WRONLY | O_CREAT, 0644);
    if (IS_ERR(output_file)) {
        return PTR_ERR(output_file);
    }

    bytes_written = kernel_write(output_file, data, data_size, &file_pos);
    if (bytes_written < 0) {
        filp_close(output_file, NULL);
        return bytes_written;
    }

    filp_close(output_file, NULL);
    return 0;
}

SYSCALL_DEFINE4(xor_decrypt, const char __user *, input_file, const char __user *, output_file, int, num_threads, const char __user *, key_file) {
    size_t data_size, key_size;
    unsigned char *data_buffer, *key_buffer;
    struct task_struct **thread_handles;
    decrypt_thread_args_t *thread_args;
    size_t chunk_size, offset;
    int i, result = 0;

    char input_file_path[256], output_file_path[256], key_file_path[256];
    if (copy_from_user(input_file_path, input_file, sizeof(input_file_path))) return -EFAULT;
    if (copy_from_user(output_file_path, output_file, sizeof(output_file_path))) return -EFAULT;
    if (copy_from_user(key_file_path, key_file, sizeof(key_file_path))) return -EFAULT;

    data_buffer = load_data(input_file_path, &data_size);
    if (!data_buffer) {
        return -EIO;
    }

    key_buffer = load_key(key_file_path, &key_size);
    if (!key_buffer) {
        kfree(data_buffer);
        return -EIO;
    }

    thread_handles = kmalloc_array(num_threads, sizeof(struct task_struct *), GFP_KERNEL);
    thread_args = kmalloc_array(num_threads, sizeof(decrypt_thread_args_t), GFP_KERNEL);
    if (!thread_handles || !thread_args) {
        kfree(data_buffer);
        kfree(key_buffer);
        return -ENOMEM;
    }

    chunk_size = data_size / num_threads;
    offset = 0;

    for (i = 0; i < num_threads; ++i) {
        thread_args[i].buffer = data_buffer;
        thread_args[i].key = key_buffer;
        thread_args[i].key_length = key_size;
        thread_args[i].start_offset = offset;
        thread_args[i].end_offset = (i == num_threads - 1) ? data_size : (offset + chunk_size);
        offset += chunk_size;

        thread_handles[i] = kthread_run(xor_decrypt_worker, &thread_args[i], "decrypt_thread_%d", i);
        if (IS_ERR(thread_handles[i])) {
            printk(KERN_ERR "ERROR: No se pudo iniciar el hilo %d\n", i);
            result = PTR_ERR(thread_handles[i]);
            goto cleanup;
        }
    }

    for (i = 0; i < num_threads; ++i) {
        if (thread_handles[i]) {
            int thread_status = kthread_stop(thread_handles[i]);
            if (thread_status != 0 && thread_status != -EINTR) {
                printk(KERN_ERR "ERROR: El hilo %d terminó con error %d\n", i, thread_status);
            } else {
                printk(KERN_INFO "Hilo %d finalizó correctamente\n", i);
            }
        }
    }
    result = save_data(output_file_path, data_buffer, data_size);

cleanup:
    kfree(data_buffer);
    kfree(key_buffer);
    kfree(thread_handles);
    kfree(thread_args);

    return result;
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

```
{
	"memoryInfo":	{
		"free":	1605672960,
		"used":	3474001920,
		"cached":	1088667648
	},
	"swapInfo":	{
		"free":	420416,
		"used":	103871
	},
	"pageFaults":	{
		"minors":	16933,
		"majors":	0
	},
	"activeInactivePages":	{
		"active":	177540,
		"inactive":	470811
	},
	"topProcesses":	[{
			"pid":	5416,
			"name":	"node",
			"memoryUsage":	390553600
		}, {
			"pid":	2699,
			"name":	"code",
			"memoryUsage":	243548160
		}, {
			"pid":	5215,
			"name":	"chrome",
			"memoryUsage":	211554304
		}, {
			"pid":	3603,
			"name":	"chrome",
			"memoryUsage":	193298432
		}, {
			"pid":	5670,
			"name":	"code",
			"memoryUsage":	153034752
		}]
}
```

## Configurar el kernel

Primero ingrasamos al directorio del codigo fuente.

```
$ cd linux-6.6.54
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