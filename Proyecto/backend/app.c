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

#define SYS_get_memory_info         454
#define SYS_get_swap_info           455
#define SYS_get_page_faults         456
#define SYS_get_act_inact_pages     457
#define SYS_get_memory_processes    458

void get_memory_info(unsigned long *mem_free, unsigned long *mem_used, unsigned long *mem_cached) {
    long result = syscall(SYS_get_memory_info, mem_free, mem_used, mem_cached);
    if (result != 0) {
        perror("Error getting memory info");
    }
}

void get_swap_info(unsigned long *swap_free, unsigned long *swap_used) {
    long result = syscall(SYS_get_swap_info, swap_free, swap_used);
    if (result != 0) {
        perror("Error getting swap info");
    }
}

void get_page_faults(unsigned long *minor_faults, unsigned long *major_faults) {
    long result = syscall(SYS_get_page_faults, minor_faults, major_faults);
    if (result != 0) {
        perror("Error getting page faults");
    }
}

void get_act_inact_pages(unsigned long *active_pages, unsigned long *inactive_pages) {
    long result = syscall(SYS_get_act_inact_pages, active_pages, inactive_pages);
    if (result != 0) {
        perror("Error getting active/inactive pages");
    }
}

void get_memory_processes(pid_t pids[], unsigned long mem_usage[], char names[][TASK_COMM_LEN]) {
    long result = syscall(SYS_get_memory_processes, pids, mem_usage, names);
    if (result != 0) {
        perror("Error getting memory processes");
    }
}

void send_json_response(int client_socket) {
    unsigned long mem_free, mem_used, mem_cached;
    unsigned long swap_free, swap_used;
    unsigned long minor_faults, major_faults;
    unsigned long active_pages, inactive_pages;
    pid_t pids[TOP_PROCESSES];
    unsigned long mem_usage[TOP_PROCESSES];
    char names[TOP_PROCESSES][TASK_COMM_LEN];

    // Llamadas a las syscalls
    get_memory_info(&mem_free, &mem_used, &mem_cached);
    get_swap_info(&swap_free, &swap_used);
    get_page_faults(&minor_faults, &major_faults);
    get_act_inact_pages(&active_pages, &inactive_pages);
    get_memory_processes(pids, mem_usage, names);

    // Crear el JSON
    cJSON *json = cJSON_CreateObject();

    // Información de memoria
    cJSON *memory_info = cJSON_CreateObject();
    cJSON_AddNumberToObject(memory_info, "free", mem_free);
    cJSON_AddNumberToObject(memory_info, "used", mem_used);
    cJSON_AddNumberToObject(memory_info, "cached", mem_cached);
    cJSON_AddItemToObject(json, "memoryInfo", memory_info);

    // Información de swap
    cJSON *swap_info = cJSON_CreateObject();
    cJSON_AddNumberToObject(swap_info, "free", swap_free);
    cJSON_AddNumberToObject(swap_info, "used", swap_used);
    cJSON_AddItemToObject(json, "swapInfo", swap_info);

    // Fallos de página
    cJSON *page_faults = cJSON_CreateObject();
    cJSON_AddNumberToObject(page_faults, "minors", minor_faults);
    cJSON_AddNumberToObject(page_faults, "majors", major_faults);
    cJSON_AddItemToObject(json, "pageFaults", page_faults);

    // Páginas activas/inactivas
    cJSON *active_inactive_pages = cJSON_CreateObject();
    cJSON_AddNumberToObject(active_inactive_pages, "active", active_pages);
    cJSON_AddNumberToObject(active_inactive_pages, "inactive", inactive_pages);
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