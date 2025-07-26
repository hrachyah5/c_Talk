/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
int sock;

void *receive_msg(void *arg)
{
	char buffer[1024];
	while(1)
	{
		int val = read(sock, buffer, 1024);
		if(val > 0)
		{
			buffer[val] = '\0';
			printf("Message: %s\n", buffer);
			fflush(stdout);
		}
	}

	return NULL;
}

int main()
{
	struct sockaddr_in serv_addr;
	char buffer[1024];
	pthread_t recv_thread;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
		
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	printf("Connected to server with port %d\n", PORT);	
	
	pthread_create(&recv_thread, NULL, receive_msg, NULL);
	
	while(1)
	{
		printf("> ");
		fgets(buffer, sizeof(buffer), stdin);
		send(sock, buffer, strlen(buffer), 0);
	}	

	return 0;
}*/

/*


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 20

int sock;
pthread_t recv_thread;
int running = 1;
char nickname[BUFFER_SIZE] = {0};
char message_history[HISTORY_SIZE][BUFFER_SIZE];
int history_index = 0;

void save_message(const char *msg)
{
    strncpy(message_history[history_index], msg, BUFFER_SIZE - 1);
    history_index = (history_index + 1) % HISTORY_SIZE;
}

void *receive_msg(void *arg)
{
    char buffer[BUFFER_SIZE];
    while (running)
    {
        int val = read(sock, buffer, BUFFER_SIZE);
        if (val > 0)
        {
            buffer[val] = '\0';
            printf("\nMessage: %s\n> ", buffer);
            fflush(stdout);
        }
        else if (val == 0)
        {
            printf("\nServer disconnected.\n");
            running = 0;
            break;
        }
        else if (val == -1 && errno != EAGAIN)
        {
            perror("Error reading from socket");
            running = 0;
            break;
        }
        usleep(100000);
    }
    return NULL;
}

void cleanup(int sig)
{
    printf("\nTerminating client...\n");
    running = 0;
    close(sock);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    exit(0);
}

void print_history()
{
    printf("\nMessage History:\n");
    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        if (strlen(message_history[i]) > 0)
        {
            printf("%d: %s\n", i + 1, message_history[i]);
        }
    }
    printf("\n");
}

void set_nickname()
{
    printf("Enter your nickname: ");
    fgets(nickname, sizeof(nickname), stdin);
    nickname[strcspn(nickname, "\n")] = '\0';  // Remove newline
    printf("Nickname set to: %s\n", nickname);
}

int main()
{
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    signal(SIGINT, cleanup);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    fcntl(sock, F_SETFL, O_NONBLOCK);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    set_nickname();

    pthread_create(&recv_thread, NULL, receive_msg, NULL);

    while (running)
    {
        printf("> ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n')
                buffer[strlen(buffer) - 1] = '\0'; // Remove newline

            if (strncmp(buffer, "/history", 8) == 0)
            {
                print_history();
                continue;
            }
            else if (strncmp(buffer, "/exit", 5) == 0)
            {
                running = 0;
                break;
            }
            else if (strncmp(buffer, "/sendfile", 9) == 0)
            {
                // Implement file sending
                char filename[BUFFER_SIZE];
                sscanf(buffer + 10, "%s", filename);
                printf("Sending file: %s\n", filename);
                // TODO: Implement file sending
                continue;
            }
            else
            {
                save_message(buffer);
                char msg_to_send[BUFFER_SIZE];
                snprintf(msg_to_send, sizeof(msg_to_send), "[%s]: %s", nickname, buffer);
                if (send(sock, msg_to_send, strlen(msg_to_send), 0) < 0)
                {
                    perror("Send failed");
                    running = 0;
                }
            }
        }
        usleep(100000);
    }

    cleanup(0);
    return 0;
}*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 20

int sock;
pthread_t recv_thread;
int running = 1;
char username[BUFFER_SIZE] = {0};  
char message_history[HISTORY_SIZE][BUFFER_SIZE];
int history_index = 0;

void save_message(const char *msg)
{
    strncpy(message_history[history_index], msg, BUFFER_SIZE - 1);
    history_index = (history_index + 1) % HISTORY_SIZE;
}

void *receive_msg(void *arg)
{
    char buffer[BUFFER_SIZE];
    while (running)
    {
        int val = read(sock, buffer, BUFFER_SIZE);
        if (val > 0)
        {
            buffer[val] = '\0';
            printf("\nMessage: %s\n> ", buffer);
            fflush(stdout);
        }
        else if (val == 0)
        {
            printf("\nServer disconnected.\n");
            running = 0;
            break;
        }
        else if (val == -1 && errno != EAGAIN)
        {
            perror("Error reading from socket");
            running = 0;
            break;
        }
        usleep(100000);
    }
    return NULL;
}

void cleanup(int sig)
{
    printf("\nTerminating client...\n");
    running = 0;
    close(sock);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    exit(0);
}

void print_history()
{
    printf("\nMessage History:\n");
    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        if (strlen(message_history[i]) > 0)
        {
            printf("%d: %s\n", i + 1, message_history[i]);
        }
    }
    printf("\n");
}

void set_username()
{
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';  

    while (strlen(username) == 0)
    {
        printf("Username cannot be empty. Please enter a valid username: ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = '\0';
    }

    printf("Username set to: %s\n", username);
}

int main()
{
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    signal(SIGINT, cleanup);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    fcntl(sock, F_SETFL, O_NONBLOCK);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    set_username();  

    pthread_create(&recv_thread, NULL, receive_msg, NULL);

    while (running)
    {
        printf("> ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n')
                buffer[strlen(buffer) - 1] = '\0'; 

            if (strncmp(buffer, "/history", 8) == 0)
            {
                print_history();
                continue;
            }
            else if (strncmp(buffer, "/exit", 5) == 0)
            {
                running = 0;
                break;
            }
            else if (strncmp(buffer, "/sendfile", 9) == 0)
            {
                
                char filename[BUFFER_SIZE];
                sscanf(buffer + 10, "%s", filename);
                printf("Sending file: %s\n", filename);
                
                continue;
            }
            else
            {
                save_message(buffer);
                char msg_to_send[BUFFER_SIZE];
                snprintf(msg_to_send, sizeof(msg_to_send), "[%s]: %s", username, buffer); 
                if (send(sock, msg_to_send, strlen(msg_to_send), 0) < 0)
                {
                    perror("Send failed");
                    running = 0;
                }
            }
        }
        usleep(100000);
    }

    cleanup(0);
    return 0;
}






