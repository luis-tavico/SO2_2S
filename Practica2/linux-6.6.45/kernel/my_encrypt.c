#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

// Estructura para pasar argumentos a los hilos
typedef struct {
    unsigned char *data;
    unsigned char *key;
    size_t start;
    size_t end;
    size_t key_len;
} thread_data_t;

// Función que realiza el cifrado XOR en una porción de datos
int xorcipher_kthread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    size_t i;
    printk(KERN_INFO "Hilo %s: Iniciando cifrado XOR desde %zu hasta %zu\n", current->comm, data->start, data->end);
    for (i = data->start; i < data->end; ++i) {
        data->data[i] ^= data->key[i % data->key_len];
    }
    printk(KERN_INFO "Hilo %s: Cifrado completado\n", current->comm);
    return 0;
}

// Función para leer un archivo de clave desde el kernel
unsigned char *read_key_from_file(const char *key_file, size_t *key_size) {
    struct file *key_fp;
    loff_t pos = 0;
    unsigned char *key_data;

    key_fp = filp_open(key_file, O_RDONLY, 0);
    if (IS_ERR(key_fp)) {
        printk(KERN_ERR "ERROR: No se pudo abrir el archivo de clave: %s\n", key_file);
        return NULL;
    }

    *key_size = i_size_read(file_inode(key_fp));
    if (*key_size <= 0) {
        printk(KERN_ERR "ERROR: Tamaño del archivo de clave inválido\n");
        filp_close(key_fp, NULL);
        return NULL;
    }

    key_data = kmalloc(*key_size + 1, GFP_KERNEL);
    if (!key_data) {
        printk(KERN_ERR "ERROR: No se pudo reservar memoria para la clave\n");
        filp_close(key_fp, NULL);
        return NULL;
    }

    kernel_read(key_fp, key_data, *key_size, &pos);
    key_data[*key_size] = '\0';  // Añadir terminador nulo
    filp_close(key_fp, NULL);
    return key_data;
}

// Función para leer un archivo en memoria
unsigned char *read_file(const char *filename, size_t *file_size) {
    struct file *fp;
    loff_t pos = 0;
    unsigned char *data;

    fp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(fp)) {
        printk(KERN_ERR "ERROR: No se pudo abrir el archivo de entrada: %s\n", filename);
        return NULL;
    }

    *file_size = i_size_read(file_inode(fp));
    if (*file_size <= 0) {
        printk(KERN_ERR "ERROR: Tamaño del archivo de entrada inválido\n");
        filp_close(fp, NULL);
        return NULL;
    }

    data = kmalloc(*file_size, GFP_KERNEL);
    if (!data) {
        printk(KERN_ERR "ERROR: No se pudo reservar memoria para el archivo\n");
        filp_close(fp, NULL);
        return NULL;
    }

    kernel_read(fp, data, *file_size, &pos);
    filp_close(fp, NULL);
    return data;
}

// Función para escribir datos en un archivo
int write_file(const char *filename, unsigned char *data, size_t size) {
    struct file *fp;
    loff_t pos = 0;
    ssize_t written;

    fp = filp_open(filename, O_WRONLY | O_CREAT, 0644);
    if (IS_ERR(fp)) {
        printk(KERN_ERR "ERROR: No se pudo abrir el archivo de salida: %s\n", filename);
        return PTR_ERR(fp);
    }

    written = kernel_write(fp, data, size, &pos);
    if (written < 0) {
        printk(KERN_ERR "ERROR: Fallo al escribir en el archivo de salida\n");
        filp_close(fp, NULL);
        return written;
    }

    filp_close(fp, NULL);
    return 0;
}

// Syscall para encriptar utilizando hilos del kernel
SYSCALL_DEFINE4(my_encrypt, const char __user *, input_file, const char __user *, output_file, int, num_threads, const char __user *, key_file) {
    size_t file_size, key_size;
    unsigned char *file_data, *key_data;
    struct task_struct **threads;
    thread_data_t *thread_data;
    size_t chunk_size, start;
    int i, ret;

    // Convertir las rutas desde el espacio de usuario al espacio del kernel
    char kernel_input_file[256], kernel_output_file[256], kernel_key_file[256];
    if (copy_from_user(kernel_input_file, input_file, sizeof(kernel_input_file))) return -EFAULT;
    if (copy_from_user(kernel_output_file, output_file, sizeof(kernel_output_file))) return -EFAULT;
    if (copy_from_user(kernel_key_file, key_file, sizeof(kernel_key_file))) return -EFAULT;

    // Leer el archivo de entrada
    file_data = read_file(kernel_input_file, &file_size);
    if (!file_data) {
        return -EIO;
    }

    // Leer el archivo de clave
    key_data = read_key_from_file(kernel_key_file, &key_size);
    if (!key_data) {
        kfree(file_data);
        return -EIO;
    }

    // Reservar memoria para los hilos y sus datos
    threads = kmalloc_array(num_threads, sizeof(struct task_struct *), GFP_KERNEL);
    thread_data = kmalloc_array(num_threads, sizeof(thread_data_t), GFP_KERNEL);
    if (!threads || !thread_data) {
        kfree(file_data);
        kfree(key_data);
        return -ENOMEM;
    }

    // Dividir el trabajo entre los hilos
    chunk_size = file_size / num_threads;
    start = 0;

    for (i = 0; i < num_threads; ++i) {
        thread_data[i].data = file_data;
        thread_data[i].key = key_data;
        thread_data[i].key_len = key_size;
        thread_data[i].start = start;
        thread_data[i].end = (i == num_threads - 1) ? file_size : (start + chunk_size);  // Último hilo toma el resto
        start += chunk_size;

        printk(KERN_INFO "Iniciando hilo %d para el cifrado de bytes %zu a %zu\n", i, thread_data[i].start, thread_data[i].end);

        threads[i] = kthread_run(xorcipher_kthread, &thread_data[i], "xor_thread_%d", i);
        if (IS_ERR(threads[i])) {
            printk(KERN_ERR "ERROR: No se pudo crear el hilo %d\n", i);
            ret = PTR_ERR(threads[i]);
            goto cleanup;
        }
    }

    // Esperar a que todos los hilos terminen
    for (i = 0; i < num_threads; ++i) {
        if (threads[i]) {
            kthread_stop(threads[i]);
        }
    }

    // Escribir el archivo de salida
    ret = write_file(kernel_output_file, file_data, file_size);

cleanup:
    // Liberar memoria
    kfree(file_data);
    kfree(key_data);
    kfree(threads);
    kfree(thread_data);

    return ret;
}

