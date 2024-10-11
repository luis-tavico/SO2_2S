#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cjson/cJSON.h>

#define PORT 8080
#define TOP_PROCESSES 5
#define TASK_COMM_LEN 16

#define SYS_obtener_info_memoria        454
#define SYS_obtener_info_swap           455
#define SYS_obtener_fallos_pagina       456
#define SYS_obtener_paginas_act_inact   457
#define SYS_obtener_procesos_memoria    458

void get_memory_info(unsigned long *memoria_libre, unsigned long *memoria_usada, unsigned long *memoria_cacheada) {
    long result = syscall(SYS_obtener_info_memoria, memoria_libre, memoria_usada, memoria_cacheada);
    if (result != 0) {
        perror("Error getting memory info");
    }
}

void get_swap_info(unsigned long *swap_libre, unsigned long *swap_usada) {
    long result = syscall(SYS_obtener_info_swap, swap_libre, swap_usada);
    if (result != 0) {
        perror("Error getting swap info");
    }
}

void get_page_faults(unsigned long *fallos_menores, unsigned long *fallos_mayores) {
    long result = syscall(SYS_obtener_fallos_pagina, fallos_menores, fallos_mayores);
    if (result != 0) {
        perror("Error getting page faults");
    }
}

void get_act_inact_pages(unsigned long *paginas_activas, unsigned long *paginas_inactivas) {
    long result = syscall(SYS_obtener_paginas_act_inact, paginas_activas, paginas_inactivas);
    if (result != 0) {
        perror("Error getting active/inactive pages");
    }
}

void get_memory_processes(pid_t pids[], unsigned long mem_usage[], char names[][TASK_COMM_LEN]) {
    long result = syscall(SYS_obtener_procesos_memoria, pids, mem_usage, names);
    if (result != 0) {
        perror("Error getting memory processes");
    }
}

void send_json_response(int client_socket) {
    unsigned long memoria_libre, memoria_usada, memoria_cacheada;
    unsigned long swap_libre, swap_usada;
    unsigned long fallos_menores, fallos_mayores;
    unsigned long paginas_activas, paginas_inactivas;
    pid_t pids[TOP_PROCESSES];
    unsigned long mem_usage[TOP_PROCESSES];
    char names[TOP_PROCESSES][TASK_COMM_LEN];

    // Llamadas a las syscalls
    get_memory_info(&memoria_libre, &memoria_usada, &memoria_cacheada);
    get_swap_info(&swap_libre, &swap_usada);
    get_page_faults(&fallos_menores, &fallos_mayores);
    get_act_inact_pages(&paginas_activas, &paginas_inactivas);
    get_memory_processes(pids, mem_usage, names);

    // Crear el JSON
    cJSON *json = cJSON_CreateObject();

    // Información de memoria
    cJSON *memory_info = cJSON_CreateObject();
    cJSON_AddNumberToObject(memory_info, "free", memoria_libre);
    cJSON_AddNumberToObject(memory_info, "used", memoria_usada);
    cJSON_AddNumberToObject(memory_info, "cached", memoria_cacheada);
    cJSON_AddItemToObject(json, "memoryInfo", memory_info);

    // Información de swap
    cJSON *swap_info = cJSON_CreateObject();
    cJSON_AddNumberToObject(swap_info, "free", swap_libre);
    cJSON_AddNumberToObject(swap_info, "used", swap_usada);
    cJSON_AddItemToObject(json, "swapInfo", swap_info);

    // Fallos de página
    cJSON *page_faults = cJSON_CreateObject();
    cJSON_AddNumberToObject(page_faults, "minors", fallos_menores);
    cJSON_AddNumberToObject(page_faults, "majors", fallos_mayores);
    cJSON_AddItemToObject(json, "pageFaults", page_faults);

    // Páginas activas/inactivas
    cJSON *active_inactive_pages = cJSON_CreateObject();
    cJSON_AddNumberToObject(active_inactive_pages, "active", paginas_activas);
    cJSON_AddNumberToObject(active_inactive_pages, "inactive", paginas_inactivas);
    cJSON_AddItemToObject(json, "activeInactivePages", active_inactive_pages);

    // Procesos con mayor uso de memoria
    cJSON *processes = cJSON_CreateArray();
    for (int i = 0; i < TOP_PROCESSES; i++) {
        cJSON *process = cJSON_CreateObject();
        cJSON_AddNumberToObject(process, "pid", pids[i]);
        cJSON_AddStringToObject(process, "name", names[i]);
        cJSON_AddNumberToObject(process, "memoryUsage", mem_usage[i]);
        cJSON_AddItemToArray(processes, process);
    }
    cJSON_AddItemToObject(json, "topProcesses", processes);

    // Convertir JSON a cadena
    char *json_string = cJSON_Print(json);

    // Enviar la respuesta HTTP con el encabezado correcto
    const char *header = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    send(client_socket, header, strlen(header), 0);
    send(client_socket, json_string, strlen(json_string), 0);

    // Liberar memoria
    free(json_string);
    cJSON_Delete(json);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    // Crear el socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Enlazar el socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(server_socket);
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, 10) < 0) {
        perror("Error listening");
        close(server_socket);
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        send_json_response(client_socket);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}