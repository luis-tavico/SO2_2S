savedcmd_kernel/power/built-in.a := rm -f kernel/power/built-in.a;  printf "kernel/power/%s " qos.o main.o console.o process.o suspend.o hibernate.o snapshot.o swap.o user.o wakelock.o poweroff.o energy_model.o | xargs ar cDPrST kernel/power/built-in.a
