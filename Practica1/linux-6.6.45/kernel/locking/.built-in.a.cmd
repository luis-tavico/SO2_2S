savedcmd_kernel/locking/built-in.a := rm -f kernel/locking/built-in.a;  printf "kernel/locking/%s " mutex.o semaphore.o rwsem.o percpu-rwsem.o spinlock.o osq_lock.o qspinlock.o rtmutex_api.o qrwlock.o | xargs ar cDPrST kernel/locking/built-in.a
