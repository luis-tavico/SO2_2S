#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

#define MY_ENCRYPT_SYSCALL 548

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

    // Ciclo que sigue pidiendo comandos hasta que se ingrese "salir"
    while (1) {
        // Solicitar entrada del usuario
        printf("> ");
        fgets(comando, sizeof(comando), stdin);

        // Eliminar el salto de línea al final de la entrada
        comando[strcspn(comando, "\n")] = 0;

        // Si el usuario ingresa "salir", finalizar el programa
        if (strcmp(comando, "salir") == 0) {
            printf("Terminando el programa.\n");
            break;
        }

        // Separar los argumentos del comando ingresado
        char *argumentos[10]; // Capacidad para hasta 10 argumentos
        int num_argumentos = 0;
        char *token = strtok(comando, " ");
        while (token != NULL && num_argumentos < 10) {
            argumentos[num_argumentos++] = token;
            token = strtok(NULL, " ");
        }

        // Inicializar valores
        archivo_entrada = NULL;
        archivo_salida = NULL;
        archivo_clave = NULL;
        num_hilos = 0;
        error_parametros = 0;

        // Procesar los argumentos
        for (int i = 0; i < num_argumentos; i++) {
            if (strcmp(argumentos[i], "-p") == 0 && i + 1 < num_argumentos) {
                archivo_entrada = argumentos[++i];
            } else if (strcmp(argumentos[i], "-o") == 0 && i + 1 < num_argumentos) {
                archivo_salida = argumentos[++i];
            } else if (strcmp(argumentos[i], "-t") == 0 && i + 1 < num_argumentos) {
                num_hilos = atoi(argumentos[++i]);
                if (num_hilos <= 0) {
                    fprintf(stderr, "Error: El parámetro -t para el número de hilos es inválido o falta (debe ser mayor que 0)\n");
                    error_parametros = 1;
                }
            } else if (strcmp(argumentos[i], "-k") == 0 && i + 1 < num_argumentos) {
                archivo_clave = argumentos[++i];
            } else {
                mostrar_uso();
                error_parametros = 1;
                break;
            }
        }

        // Verificar si faltan parámetros obligatorios
        if (!archivo_entrada) {
            fprintf(stderr, "Error: Falta el parámetro -p para el archivo de entrada\n");
            error_parametros = 1;
        }
        if (!archivo_salida) {
            fprintf(stderr, "Error: Falta el parámetro -o para el archivo de salida\n");
            error_parametros = 1;
        }
        if (!archivo_clave) {
            fprintf(stderr, "Error: Falta el parámetro -k para el archivo de clave\n");
            error_parametros = 1;
        }

        // Si hay errores en los parámetros, mostrar el uso y no ejecutar
        if (error_parametros) {
            mostrar_uso();
            continue;
        }

        printf("Iniciando encriptación...\n");
        printf("Archivo de entrada: %s\n", archivo_entrada);
        printf("Archivo de salida: %s\n", archivo_salida);
        printf("Número de hilos: %d\n", num_hilos);
        printf("Archivo de clave: %s\n", archivo_clave);

        // Realizar la llamada al sistema `my_encrypt`
        long resultado = syscall(MY_ENCRYPT_SYSCALL, archivo_entrada, archivo_salida, num_hilos, archivo_clave);

        if (resultado != 0) {
            perror("Error: Fallo en my_encrypt");
            printf("\n");
        } else {
            // Si la encriptación fue exitosa
            printf("Encriptación completada exitosamente. Archivo de salida: %s\n", archivo_salida);
            printf("\n");
        }
    }

    return 0;
}

