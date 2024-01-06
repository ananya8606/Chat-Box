#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8000

void Start_Server() {
	int server_fd, addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in server_addr, client_addr;
	char buffer[1024] = {0};

	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    	perror("socket failed");
    	exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	memset(&client_addr, 0, sizeof(client_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    	perror("bind failed");
    	exit(EXIT_FAILURE);
	}

	while (1) {
    	ssize_t recv_len = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
    	if (recv_len == -1) {
        	perror("recvfrom");
        	exit(EXIT_FAILURE);
    	}

    	printf("Message Received from %s is %s\n", inet_ntoa(client_addr.sin_addr), buffer);
    	memset(buffer, 0, sizeof(buffer)); // Clear the buffer
	}

	close(server_fd);
}

void Send_msg(const char *server_ip, const char *message) {
	int client_fd;
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};

	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    	perror("socket");
    	return;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	std::string user_message = message; // Initial message

	while (true) {
    	std::string IP;

    	std::cout << "Enter the server IP (or 'quit' to exit): ";
    	std::cin >> IP;

    	if (IP == "quit") {
        	break; // Exit the loop when the user enters "quit"
    	}

    	std::cin.ignore(); // Clear input buffer
    	std::cout << "Enter a message to send: ";
    	std::getline(std::cin, user_message);

    	serv_addr.sin_addr.s_addr = inet_addr(IP.c_str());

    	if (sendto(client_fd, user_message.c_str(), user_message.length(), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        	perror("sendto");
    	} else {
        	printf("Message sent to server: %s\n", user_message.c_str());
    	}
	}

	close(client_fd);
}


int main() {
	std::thread serverThread(Start_Server);

	std::string server_ip = ""; // Initialize the server IP

	std::thread senderThread(Send_msg, server_ip.c_str(), "Hello from Client");
	senderThread.join();

	serverThread.join();

	return 0;
}
