/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10


int main() {
    
   int server_fd;
   int new_client;
   int client_sockets[MAX_CLIENTS]={0};
   struct sockaddr_in address;
   socklen_t addrlen = sizeof(address);
   char buffer[1024];
   fd_set readfds;
   
   server_fd = socket(AF_INET,SOCK_STREAM,0);
   if(server_fd == 0){
   perror("Socket failed");
   exit(EXIT_FAILURE);
}
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(PORT);
   bind(server_fd, (struct sockaddr *)&address,sizeof(address));
   listen(server_fd,5);
   printf("Server started on port %d\n",PORT);
   

   while(1){
	FD_ZERO(&readfds);
    FD_SET(server_fd,&readfds);
	int max_fd = server_fd;
    for(int i = 0;i <MAX_CLIENTS;i++){
		int fd = client_sockets[i];
	    if(fd >0){
			FD_SET(fd,&readfds);
		}
		
		if(fd >max_fd){
			max_fd = fd;
		}
	}

    	 select(max_fd + 1,&readfds,NULL,NULL,NULL);
    	 if(FD_ISSET(server_fd,&readfds)){
		 	new_client = accept(server_fd, (struct sockaddr *)&address,&addrlen);
			printf("New connection: socket fd: %d, IP:%s\n",new_client,inet_ntoa(address.sin_addr));
			
		  		for(int i = 0;i < MAX_CLIENTS;i++){
					if(client_sockets[i] == 0){
					    client_sockets[i] = new_client;
							break;
					}

				}

		} 

		for(int i = 0;i<MAX_CLIENTS;i++){
			int fd = client_sockets[i];
			if(FD_ISSET(fd,&readfds)){
				int val = read(fd,buffer,1024);
				if(val == 0){
					getpeername(fd, (struct sockaddr *)&address,&addrlen);
					printf("Client disconnected. IP: %s \n",inet_ntoa(address.sin_addr));
					close(fd);
					client_sockets[i] = 0;
					

				} else{
					buffer[val] = '\0';
					for(int j = 0;j < MAX_CLIENTS;j++){
						if(client_sockets[j] != 0 && client_sockets[j] != fd){
							send(client_sockets[j],buffer,strlen(buffer),0);
						}
					}
				}
			}
		}
}

  

 return 0;
}
*/
/*

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>	 // sockaddr_in
#include <arpa/inet.h>

#define PORT		8080
#define MAX_CLIENTS	10

int main()
{
	int server_fd;	//socket fd for server 
	int new_client;
	int client_sockets[MAX_CLIENTS] = {0};
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
	char buffer[1024];
	fd_set readfds;
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0)
	{
		perror("Socket Failed");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	listen(server_fd, 5);
	printf("Server started of port %d\n", PORT);
	
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(server_fd, &readfds);
		int max_fd = server_fd;
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			int fd = client_sockets[i];
			if (fd > 0)
			{
				FD_SET(fd, &readfds);
			}
			if (fd > max_fd)
				max_fd = fd;
		}
		select(max_fd + 1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(server_fd, &readfds))
		{
			new_client = accept(server_fd, (struct sockaddr *)&address, &addrlen);
			printf("New connection: socket fd: %d, IP: %s\n", 
				new_client, inet_ntoa(address.sin_addr));

			for(int i = 0; i < MAX_CLIENTS; i++)
			{
				if (client_sockets[i] == 0)
				{
					client_sockets[i] = new_client;
					break;
				}
			}
		}
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			int fd = client_sockets[i];
			if (FD_ISSET(fd, &readfds))
			{
				int val = read(fd, buffer, 1024);
				if (val == 0)
				{
					getpeername(fd, (struct sockaddr *) &address, &addrlen);
					printf("Client disconnected. IP: %s\n", 
						inet_ntoa(address.sin_addr));
					close(fd);
					client_sockets[i] = 0;
				}
				else
				{
					buffer[val] = '\0';
					for(int j = 0; j < MAX_CLIENTS; j++)
					{
						if (client_sockets[j] != 0 && 
							client_sockets[j] != fd)
						{
							send(client_sockets[j], buffer, 
									strlen(buffer), 0);
						}
					}
				}
			}
		}

	}

	return 0;
}


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int server_fd;
int client_sockets[MAX_CLIENTS] = {0};


void handle_sigint(int sig) {
    printf("\nShutting down server...\n");
    close(server_fd);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            close(client_sockets[i]);
        }
    }
    exit(0);
}


int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}

int main() {
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    
    signal(SIGINT, handle_sigint);

   
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    
    if (set_non_blocking(server_fd) == -1) {
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        int max_fd = server_fd;

        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_sockets[i];
            if (fd > 0) {
                FD_SET(fd, &readfds);
            }
            if (fd > max_fd) {
                max_fd = fd;
            }
        }

        
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Select failed");
            continue;
        }

        
        if (FD_ISSET(server_fd, &readfds)) {
            int new_client = accept(server_fd, (struct sockaddr *)&address, &addrlen);
            if (new_client == -1) {
                perror("Accept failed");
                continue;
            }
            printf("New connection: socket fd: %d, IP: %s\n", new_client, inet_ntoa(address.sin_addr));

            
            if (set_non_blocking(new_client) == -1) {
                close(new_client);
                continue;
            }

            
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_client;
                    break;
                }
            }
        }

        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_sockets[i];
            if (FD_ISSET(fd, &readfds)) {
                int val = read(fd, buffer, sizeof(buffer) - 1);
                if (val == 0) {
                    
                    getpeername(fd, (struct sockaddr *)&address, &addrlen);
                    printf("Client disconnected. IP: %s\n", inet_ntoa(address.sin_addr));
                    close(fd);
                    client_sockets[i] = 0;
                } else if (val > 0) {
                    buffer[val] = '\0';
                    printf("Received message from client %d: %s\n", fd, buffer);

                    
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_sockets[j] != 0 && client_sockets[j] != fd) {
                            send(client_sockets[j], buffer, strlen(buffer), 0);
                        }
                    }
                } else {
                    perror("Read failed");
                }
            }
        }
    }

    return 0;
}



