#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <linux/uaccess.h>

#define MAX_MESSAGE_LENGTH 1024

char log_messages[MAX_LOG_ENTRIES][MAX_MESSAGE_LENGTH];
int log_count = 0;

void add_log(const char* message) {
    if (log_count < MAX_LOG_ENTRIES) {
        strcpy(log_messages[log_count], message);
        log_count++;
    } else {
        // Shift existing log messages to make space for the new one
        for (int i = 0; i < MAX_LOG_ENTRIES - 1; i++) {
            strcpy(log_messages[i], log_messages[i+1]);
        }
        strcpy(log_messages[MAX_LOG_ENTRIES - 1], message);
    }
}

//add function that hndel clinet:
void handle_client(int client_socket){
	char buffer[MAX_MESSAGE_LENGTH] = {0}; 
	int valread;

	while(valread = read(new_socket, buffer,  MAX_MESSAGE_LENGTH) > 0){
		add_log(buffer);
	}
}

int main(){

	int opt = 1;
	struct sockaddr_in address;

//Creating socket file discriptor
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if(sockfd < 0){
	perror("socket failed");
	exir(EXIT_FAILURE);
	}

//Prevents error such as: “address already in use”.
	if (setsockopt(sockfd,SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt failed");
        exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

//Bind
    if (int bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0){
    	perror("bind failed");
        exit(EXIT_FAILURE);
    }


//Listen
	if(int listen(sockfd ,3) < 0){
		perror("listen");
        exit(EXIT_FAILURE);
	}

//Accept
	if(int new_socket = accept(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("accept");
        exit(EXIT_FAILURE);
	}

	handle_client(new_socket);
	close(sockfd);

	return 0;
}
