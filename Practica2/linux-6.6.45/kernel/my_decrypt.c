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