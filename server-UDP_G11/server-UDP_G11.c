#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h> /* for memset() */
#define ECHOMAX 255
#define PORT 48000

void ErrorHandler(char *errorMessage) {
	printf(errorMessage);
}

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}

int main() {
	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return EXIT_FAILURE;
	}
	#endif

	int sock;
	struct sockaddr_in servAddr;
	struct sockaddr_in cliAddr;
	struct hostent *clientHost;
	int cliAddrLen;
	char echoBuffer[ECHOMAX];
	int recvMsgSize;

	// CREAZIONE DELLA SOCKET
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		ErrorHandler("socket() failed");
	// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// BIND DELLA SOCKET
	if ((bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0)
		ErrorHandler("bind() failed");

	// RICEZIONE DELLA STRINGA ECHO DAL CLIENT
	while(1) {
		printf("In attesa di dati...\n");

		cliAddrLen = sizeof(cliAddr);
		recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&cliAddr, &cliAddrLen);
		//sicurezza
		if(recvMsgSize < 0) continue;
			echoBuffer[recvMsgSize]='\0';

			if(strcmp("Hello", echoBuffer)==0){
				clientHost=gethostbyaddr((const char*)&cliAddr.sin_addr, sizeof(cliAddr.sin_addr), AF_INET);
				char* clientName = (clientHost != NULL) ? clientHost->h_name : "Sconosciuto";
				printf("Ricevuti dati dal client chiamato %s indirizzo %s\n", clientName, inet_ntoa(cliAddr.sin_addr));
				sendto(sock, "Server raggiunto", 16, 0, (struct sockaddr *)&cliAddr, sizeof(cliAddr));
			}
			else{
				printf("Stringa ricevuta da elaborare: %s\n", echoBuffer);

				char stringaSenzaVocali[ECHOMAX];
				int j=0;

				//eliminazione vocali

				for(int i=0;i<recvMsgSize;i++){
					char c= echoBuffer[i];
					if(c!='a'&& c!='e'&&c!='i'&&c!='o'&&c!='u'&&c!='A'&&c!='E'&&c!='I'&&c!='O'&&c!='U')
						stringaSenzaVocali[j++]=c;
				}
				stringaSenzaVocali[j]='\0';

				// Invio della stringa elaborata al client
				if (sendto(sock, stringaSenzaVocali, strlen(stringaSenzaVocali), 0, (struct sockaddr *)&cliAddr, sizeof(cliAddr)) < 0)
					ErrorHandler("sendto() failed");

				printf("Stringa elaborata inviata: %s\n", stringaSenzaVocali);
			}
			//closesocket(sock);
	}
}
