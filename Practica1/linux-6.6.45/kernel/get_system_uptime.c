#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/timekeeping.h>

SYSCALL_DEFINE0(get_system_uptime)
{
    unsigned long uptime = ktime_get_boottime_seconds();
    return uptime;
}