#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/timekeeping.h>

SYSCALL_DEFINE0(get_current_time)
{
    unsigned long epoch_time = ktime_get_real_seconds();
    return epoch_time;
}
