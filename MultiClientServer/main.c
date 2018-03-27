#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#define MAXCLIENT 2
#define BUFFLEN 1024

int clientSockets[MAXCLIENT];


int findemptyuser(int c_sockets[]){
    int i;
    for (i = 0; i <  MAXCLIENT; i++){
        if (c_sockets[i] == -1){
            return i;
        }
    }
    return -1;
}

void *connection(void *client_socket) {
    int sock = *(int*) client_socket;
    int i;
    int x = 5, y = 5, snake_length = 1;

    int dest_socket;
    if(sock == clientSockets[0]){
        dest_socket = clientSockets[1];
    }
    else{
        dest_socket = clientSockets[0];
    }
    
    send(sock, &x, BUFFLEN, 0);
    send(sock, &y, BUFFLEN, 0);
    // send(sock, &snake_length, BUFFLEN, 0);
    char event[BUFFLEN];

    int r_len = recv(sock, &event, BUFFLEN, 0);

    if (r_len == 0) {
        printf("Client %d disconnected", sock);
    }
    
    if (r_len == -1){
        printf("Recieving failed from client %d\n", sock);
    }           

    while (r_len > 0) {
        send(dest_socket, &event, BUFFLEN, 0);
    }

    close(sock);
    return 0;
}

int main()
{
    unsigned int clientaddrlen;
    int serverSocket;
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;

    pthread_t thread;

    int i;
    
    if ((serverSocket = socket(AF_INET, SOCK_STREAM,0)) < 0){
        fprintf(stderr, "ERROR #1: cannot create server socket.\n");
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(8888);

    if (bind (serverSocket, (struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        fprintf(stderr,"ERROR #2: bind server socket.\n");
        return -1;
    }

    if (listen(serverSocket, 5) <0){
        fprintf(stderr,"ERROR #3: error in listen().\n");
        return -1;
    }

    printf("The server is listening...\n"); 

    for (i = 0; i < MAXCLIENT; i++){
        clientSockets[i] = -1;
    }
    while(1){
        int client_id = findemptyuser(clientSockets);
        if (client_id != -1){
            clientaddrlen = sizeof(clientaddr);
            memset(&clientaddr, 0, clientaddrlen);
            clientSockets[client_id] = accept(serverSocket, (struct sockaddr*)&clientaddr, &clientaddrlen);
            printf("Connected:  %s %d\n",inet_ntoa(clientaddr.sin_addr), client_id);
        }else{
            printf("Connection failed: %s\n", inet_ntoa(clientaddr.sin_addr));
            return -1;
        }
        int nClient = 0;
        for(i = 0; i < MAXCLIENT; i++){
            if(clientSockets[i] != -1)
                nClient++;
        }
        if(nClient == 2){
            pthread_create(&thread, NULL, connection, &clientSockets[client_id]);
            nClient = 0;
        }
        
    }

    return 0;
}
