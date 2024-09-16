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