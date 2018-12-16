// original blog post: https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
// original implementation: https://gist.github.com/SkrewEverything/2c535e83a3a7b8e5b7aa490009a87fbb


#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
#define RESPONSE_LEN 10000

int get_file_size(char*);
void read_file(char*, int, char*);

int main(int argc, const char * argv[]){
    char *file_path = "index.html";
    //int file_size = get_file_size(file_path);
    //printf("file_size: %d\n", file_size);
    //char *buf = (char *)malloc(sizeof(char)*file_size+1);
    //memset(buf, 0x00, sizeof(char)*file_size+1);
    //read_file(buf, sizeof(char)*file_size+1, file_path);
    //char *body;
    //body = &buf[0];

    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address); // memory size of type or variable
    //char *response = "Hello, from server.";
    //char response[RESPONSE_LEN] = {0};
    //char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
    //char *body = "<!DOCTYPE html><html><head><meta charset=utf-8><title>simeple HTTP server</title></head><body><h1>Hello, from server.</h1></body></html>";
    //int content_len = strlen(body);
    //snprintf(response, RESPONSE_LEN, "%s%d\n\n%s", header, content_len, body);

    // Creating socket file descriptor
    // socket( domain, type, protocol )
    // all the parameters as well as the return value are integers.
    // 
    // domain, or address familiy:
    // AF_INET  -> IP
    // AF_INET6 -> IPv6
    // AF_UNIX  -> local channel, similar to pipes
    // AF_NS    -> Xerox Network Systems Protocols
    //
    // type:
    // SOCK_STREAM -> virtual circuit service
    // SOCK_DGRAM  -> datagram service
    // SOCK_RAW    -> direct IP service
    //
    // protocol:
    // indicate specific protocol to use in supporting the sockets operation.
    // some families may have more than one protocol to support a given type of service.
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if(listen(server_fd, 10) < 0){
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1){
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0){
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        // read resource
        int file_size = get_file_size(file_path);
        printf("file_size: %d\n", file_size);
        char *body = (char *)malloc(sizeof(char)*file_size+1);
        memset(body, 0x00, sizeof(char)*file_size+1);
        read_file(body, sizeof(char)*file_size+1, file_path);

        // create response
        char response[RESPONSE_LEN] = {0};
        char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        //char *body = "<!DOCTYPE html><html><head><meta charset=utf-8><title>simeple HTTP server</title></head><body><h1>Hello, from server.</h1></body></html>";
        int content_len = strlen(body);
        snprintf(response, RESPONSE_LEN, "%s%d\n\n%s", header, content_len, body);

        // respond
        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        write(new_socket, response, strlen(response));
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
    }
    return 0;
}

int get_file_size(char * file_path){
    struct stat statbuf;
    if(stat(file_path, &statbuf) == 0)
        return statbuf.st_size;

    perror("In get_file_size");
    exit(EXIT_FAILURE);
}

void read_file(char *buf, int buf_size, char * file_path){
    int read_size;
    FILE *fp;
    fp = fopen(file_path, "r");
    if(fp == NULL){
        perror("In fopen");
        exit(EXIT_FAILURE);
    }
    read_size = fread(buf, 1, buf_size, fp);
    if(read_size == 0){
        printf("empty file\n");
    } else{
        printf("read size: %d\n", read_size);
        buf[read_size] = '\0';
    }
    fclose(fp);
}
