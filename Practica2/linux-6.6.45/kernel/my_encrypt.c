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