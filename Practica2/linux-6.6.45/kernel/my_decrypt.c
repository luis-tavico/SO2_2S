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