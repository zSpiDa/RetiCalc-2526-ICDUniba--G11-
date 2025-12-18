#if defined _WIN32
#include <winsock2.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFERSIZE 512
#define PROTOPORT 27015
#define QLEN 6

void ErrorHandler(char *errorMessage) {
printf ("%s", errorMessage);
}

void ClearWinSock() {
#if defined _WIN32
WSACleanup();
#endif
}

void RimuoviVocali(char *str) {
    int i = 0, j = 0;
    while (str[i] != '\0') {
        char c = str[i];
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U'){
        } else {
            str[j] = str[i];
            j++;
        }
        i++;
    }
    str[j] = '\0';
}

int main(int argc, char *argv[]) {
	int mySock;
	struct sockaddr_in server;
	int port=PROTOPORT;
	int bytesRcvd;
	int totalBytesRcvd = 0;
	char buf[BUFFERSIZE];

	#if defined WIN32
	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
	ErrorHandler("Error at WSAStartup()\n");
	return 0;
	}
	#endif

	mySock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mySock < 0) {
	ErrorHandler("socket creation failed.\n");
	ClearWinSock();
	return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(port);
	if (bind(mySock, (struct sockaddr*) &server, sizeof(server)) < 0) {
	ErrorHandler("bind() failed.\n");
	closesocket(mySock);
	ClearWinSock();
	return -1;
	}

	if (listen (mySock, QLEN) < 0) {
	ErrorHandler("listen() failed.\n");
	closesocket(mySock);
	ClearWinSock();
	return -1;
	}

	int clientSocket;
	int clientLen;
	printf("In attesa di un client...\n");

	while (1) {
	struct sockaddr_in client;
	clientLen = sizeof(client);
	if ((clientSocket = accept(mySock, (struct sockaddr *)&client, &clientLen)) < 0) {
	ErrorHandler("accept() failed.\n");
	closesocket(mySock);
	ClearWinSock();
	return 0;
	}
	printf("Ricevuti dati dal client con indirizzo: %s\n", inet_ntoa(client.sin_addr));

	if ((bytesRcvd = recv(clientSocket, buf, BUFFERSIZE - 1, 0)) <= 0) {
			ErrorHandler("recv() failed or connection closed prematurely");
			closesocket(clientSocket);
			ClearWinSock();
			return -1;
	}
	else{
		buf[bytesRcvd] = '\0';
	}

	printf("Received: ");
	if ((bytesRcvd = recv(clientSocket, buf, BUFFERSIZE - 1, 0)) <= 0) {
		ErrorHandler("recv() failed or connection closed prematurely");
		closesocket(clientSocket);
		ClearWinSock();
		return -1;
	}
	else{
		totalBytesRcvd += bytesRcvd;
		buf[bytesRcvd] = '\0';
		printf("%s\n", buf);
		RimuoviVocali(buf);
		int msgLen = strlen(buf);
		if (send(clientSocket, buf, msgLen, 0) != msgLen) {
			 ErrorHandler("send() sent a different number of bytes than expected");
			 closesocket(clientSocket);
			 ClearWinSock();
			 return -1;
		}
	}

	 closesocket(clientSocket);
	}
	closesocket(mySock);
	    ClearWinSock();
	    return 0;
}
