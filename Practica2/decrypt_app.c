#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <errno.h>

#define SYS_my_decrypt 441

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s -p <ruta_archivo_entrada> -o <ruta_archivo_salida> -j <numero_de_hilos> -k <ruta_archivo_clave>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ruta_archivo_entrada = argv[2];
    char *ruta_archivo_salida = argv[4];
    int numero_de_hilos = atoi(argv[6]);
    char *ruta_archivo_clave = argv[8];

    long res = syscall(SYS_my_decrypt, ruta_archivo_entrada, ruta_archivo_salida, numero_de_hilos, ruta_archivo_clave);
    if (res != 0) {
        perror("syscall my_decrypt falló");
        return EXIT_FAILURE;
    }

    printf("Descifrado completado exitosamente.\n");
    return EXIT_SUCCESS;
}