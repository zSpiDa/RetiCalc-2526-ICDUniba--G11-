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
#define BUFFERSIZE 512
#define PROTOPORT 27015

void ErrorHandler(char *errorMessage) {
printf("%s",errorMessage);
}

void ClearWinSock() {
#if defined _WIN32
WSACleanup();
#endif
}

int main(){
	#if defined _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0) {
	printf ("error at WSASturtup\n");
	return -1;
	}
	#endif

	int cSock;
	cSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cSock < 0) {
	ErrorHandler("socket creation failed.\n");
	closesocket(cSock);
	ClearWinSock();
	return -1;
	}

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PROTOPORT);

	if (connect(cSock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
	ErrorHandler( "Failed to connect.\n" );
	closesocket(cSock);
	ClearWinSock();
	return -1;
	}
	printf("Connesso\n");

	char* inputString = "Hello";
	int stringLen = strlen(inputString);

	if (send(cSock, inputString, stringLen, 0) != stringLen) {
	ErrorHandler("send() sent a different number of bytes than expected");
	closesocket(cSock);
	ClearWinSock();
	return -1;
	}

	char string[BUFFERSIZE];
	printf("Stringa: ");
	fflush(stdin);
	fgets(string, BUFFERSIZE, stdin);
	size_t len = strlen(string);
	if (len > 0 && string[len - 1] == '\n') {
	    string[len - 1] = '\0';
	}

	stringLen = strlen(string);
	if (send(cSock, string, stringLen, 0) != stringLen) {
	ErrorHandler("send() sent a different number of bytes than expected");
	closesocket(cSock);
	ClearWinSock();
	return -1;
	}
	printf("Messaggio inviato\n");
	int bytesRcvd;
	char buf[BUFFERSIZE];
	printf("Messaggio ricevuto: ");
	if ((bytesRcvd = recv(cSock, buf, BUFFERSIZE - 1, 0)) <= 0) {
	ErrorHandler("recv() failed or connection closed prematurely");
	}
	else{
		buf[bytesRcvd] = '\0';
		printf("%s", buf);
	}
	closesocket(cSock);
	ClearWinSock();
	printf("\nProcesso terminato\n");
	system("pause");
	return(0);
}
