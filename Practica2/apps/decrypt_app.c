#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

#define MY_DECRYPT_SYSCALL 549

void mostrar_uso() {
    fprintf(stderr, "Ingresa: -p <archivo_entrada> -o <archivo_salida> -t <num_hilos> -k <archivo_clave>\nIngresa: 'salir' si deseas terminar el programa.\n");
    printf("\n");
}

int main() {
    char comando[256];
    const char *archivo_entrada = NULL;
    const char *archivo_salida = NULL;
    const char *archivo_clave = NULL;
    int num_hilos = 0;
    int error_parametros = 0;

    // Ejecutar el ciclo hasta que el usuario ingrese 'salir'
    while (1) {
        // Solicitar comando al usuario
        printf("> ");
        fgets(comando, sizeof(comando), stdin);

        // Quitar salto de línea
        comando[strcspn(comando, "\n")] = 0;

        // Verificar si el usuario quiere salir
        if (strcmp(comando, "salir") == 0) {
            printf("Saliendo del programa.\n");
            break;
        }

        // Separar la entrada en argumentos
        char *argumentos[10]; // Soporta hasta 10 argumentos
        int num_argumentos = 0;
        char *token = strtok(comando, " ");
        while (token != NULL && num_argumentos < 10) {
            argumentos[num_argumentos++] = token;
            token = strtok(NULL, " ");
        }

        // Reiniciar valores por cada iteración
        archivo_entrada = NULL;
        archivo_salida = NULL;
        archivo_clave = NULL;
        num_hilos = 0;
        error_parametros = 0;

        // Procesar los argumentos ingresados
        for (int i = 0; i < num_argumentos; i++) {
            if (strcmp(argumentos[i], "-p") == 0 && i + 1 < num_argumentos) {
                archivo_entrada = argumentos[++i];
            } else if (strcmp(argumentos[i], "-o") == 0 && i + 1 < num_argumentos) {
                archivo_salida = argumentos[++i];
            } else if (strcmp(argumentos[i], "-t") == 0 && i + 1 < num_argumentos) {
                num_hilos = atoi(argumentos[++i]);
            } else if (strcmp(argumentos[i], "-k") == 0 && i + 1 < num_argumentos) {
                archivo_clave = argumentos[++i];
            } else {
                mostrar_uso();
                continue;
            }
        }

        // Verificar parámetros obligatorios
        if (!archivo_entrada) {
            fprintf(stderr, "Error: Falta el parámetro -p para especificar el archivo de entrada\n");
            error_parametros = 1;
        }
        if (!archivo_salida) {
            fprintf(stderr, "Error: Falta el parámetro -o para el archivo de salida\n");
            error_parametros = 1;
        }
        if (num_hilos <= 0) {
            fprintf(stderr, "Error: El número de hilos especificado con -t es inválido o falta (debe ser mayor a 0)\n");
            error_parametros = 1;
        }
        if (!archivo_clave) {
            fprintf(stderr, "Error: Falta el archivo de clave, use -k para especificarlo\n");
            error_parametros = 1;
        }

        // Si algún parámetro falta, no ejecutar
        if (error_parametros) {
            mostrar_uso();
            continue;
        }

        printf("Iniciando proceso de desencriptación...\n");
        printf("Archivo de entrada: %s\n", archivo_entrada);
        printf("Archivo de salida: %s\n", archivo_salida);
        printf("Hilos utilizados: %d\n", num_hilos);
        printf("Archivo clave: %s\n", archivo_clave);

        // Llamada al sistema `my_decrypt`
        long resultado = syscall(MY_DECRYPT_SYSCALL, archivo_entrada, archivo_salida, num_hilos, archivo_clave);
        
        if (resultado != 0) {
            perror("Error al ejecutar my_decrypt");
            printf("\n");
        } else {
            printf("Desencriptación completada exitosamente\n");
            printf("\n");
        }
    }

    return 0;
}

