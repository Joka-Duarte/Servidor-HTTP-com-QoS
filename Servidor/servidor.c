#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

// Função auxiliar para identificar o tipo de arquivo sendo solicitado
const char* get_mime_type(const char* path) {
    if (strstr(path, ".html")) return "text/html; charset=UTF-8";
    if (strstr(path, ".jpg") || strstr(path, ".jpeg")) return "image/jpeg";
    if (strstr(path, ".png")) return "image/png";
    return "application/octet-stream";
}

// Função executada por cada thread para tratar um cliente específico
void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    free(arg); 
    
    pthread_detach(pthread_self());

    char buffer[4096];
    
    while (1) {
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break; 
        buffer[bytes_received] = '\0';

        // Variáveis para armazenar os componentes da requisição HTTP
        char method[16], path[256], protocol[16];
        
        // Extrai o método (GET), o caminho (/arquivo.html) e o protocolo
        if (sscanf(buffer, "%15s %255s %15s", method, path, protocol) != 3) {
            break; // Requisição mal formatada
        }

        // Se o cliente pedir a raiz "/", redireciona para o arquivo "index.html"
        if (strcmp(path, "/") == 0) {
            strcpy(path, "/index.html");
        }

        // Remove a barra inicial para procurar o arquivo na pasta atual do WSL
        char *file_path = path + 1; 

        // Tenta abrir o arquivo em modo binário de leitura ("rb")
        FILE *file = fopen(file_path, "rb");
        
        if (file == NULL) {
            // Arquivo não encontrado - Retorna Erro 404
            char not_found[] = "HTTP/1.1 404 Not Found\r\n"
                               "Content-Length: 0\r\n"
                               "Connection: keep-alive\r\n\r\n";
            send(client_fd, not_found, strlen(not_found), 0);
        } else {
            // Arquivo encontrado! Primeiro, descobre o tamanho total do arquivo
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET); // Volta o ponteiro para o início do arquivo

            const char* mime_type = get_mime_type(file_path);

            // Monta e envia os cabeçalhos HTTP com o tamanho exato do arquivo
            char headers[1024];
            snprintf(headers, sizeof(headers),
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Type: %s\r\n"
                     "Content-Length: %ld\r\n"
                     "Connection: keep-alive\r\n\r\n",
                     mime_type, file_size);
            send(client_fd, headers, strlen(headers), 0);

            // Lê o arquivo do disco em blocos (chunks) e envia para o cliente.
            // Isso impede o servidor de travar ao enviar imagens de vários megabytes.
            char file_buffer[8192];
            size_t bytes_read;
            while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
                send(client_fd, file_buffer, bytes_read, 0);
            }
            fclose(file);
        }

        if (strstr(buffer, "Connection: close") != NULL) break;
    }

    close(client_fd);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 1. Criação do socket IPv4 (AF_INET) e TCP (SOCK_STREAM)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    // Configuração para evitar o erro "Address already in use" ao reiniciar o servidor rapidamente
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080); // Porta do servidor

    // 2. Associa o socket à porta e endereço
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro no bind");
        exit(EXIT_FAILURE);
    }

    // 3. Coloca o socket em modo de escuta
    if (listen(server_fd, 10) < 0) {
        perror("Erro no listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor rodando na porta 8080...\n");

    // 4. Laço principal de aceitação de clientes
    while (1) {
        // Aloca memória dinamicamente para o descritor do cliente
        // Isso evita condições de corrida se vários clientes conectarem ao mesmo tempo
        int* client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (*client_fd < 0) {
            perror("Erro no accept");
            free(client_fd);
            continue;
        }

        // 5. Cria uma nova thread para lidar com o cliente recém-conectado
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_fd) != 0) {
            perror("Erro ao criar thread");
            free(client_fd);
        }
    }

    close(server_fd);
    return 0;
}