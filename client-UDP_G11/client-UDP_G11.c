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
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	unsigned int fromSize;
	char msg[ECHOMAX];
	char hostName[ECHOMAX];
	char echoBuffer[ECHOMAX];
	struct hostent *remoteHost;//indirizzo host remoto
	int echoStringLen;
	int respStringLen;
	int portServer;

	printf("Inserisci il nome dell'host del server da contattare\n");
	scanf("%s", hostName);	//stringa per dns
	printf("Inserisci la porta del server da contattare\n");
	scanf("%d", &portServer);

	if ((echoStringLen = strlen(hostName)) > ECHOMAX)
		ErrorHandler("Nome dell'host troppo lungo");

	//controllo porta corretta
	if(portServer<=1024||portServer>65535)
		printf("Porta non conosciuta\n");
	else
		printf("Porta riconosciuta\n");

	remoteHost=gethostbyname(hostName);
	if(remoteHost==NULL){
		ErrorHandler("Host non trovato\n");
		system("pause");
		return 0;
	}

	// CREAZIONE DELLA SOCKET
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		ErrorHandler("socket() failed");

	// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_port = htons(portServer);
	echoServAddr.sin_addr.s_addr = *((unsigned long *)remoteHost->h_addr_list[0]);

	printf("Inserisci messaggio iniziale:\n");
	scanf("%s", msg);

	// INVIO DELLA STRINGA iniziale AL SERVER
	if (sendto(sock, msg, echoStringLen, 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != echoStringLen)
		ErrorHandler("sendto() sent different number of bytes than expected");

	fromSize = sizeof(fromAddr);
	respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&fromAddr, &fromSize);
	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr){
		 fprintf(stderr, "Error: received a packet from unknown source.\n");
		exit(EXIT_FAILURE);
	}
	echoBuffer[respStringLen] = '\0';//sicurezza
	printf("Server pronto, risposta iniziale: %s\n", echoBuffer);

	printf("Inserisci una stringa di caratteri da elaborare:\n");
	fflush(stdin);
	    fgets(msg, ECHOMAX, stdin);
	    size_t len = strlen(msg);
	    if (len > 0 && msg[len - 1] == '\n') {
	    	msg[len - 1] = '\0';
	    }

	echoStringLen=len;

	// INVIO DELLA STRINGA da modificare AL SERVER
		if (sendto(sock, msg, echoStringLen, 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != echoStringLen)
			ErrorHandler("sendto() sent different number of bytes than expected");

	//ricezione stringa modificata
	respStringLen=recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&fromAddr, &fromSize);
	if (respStringLen < 0)
		ErrorHandler("recvfrom() failed");

	echoBuffer[respStringLen]='\0';

	// Recupero nome del server
	struct hostent *srvHost;
	srvHost = gethostbyaddr((const char*)&fromAddr.sin_addr, sizeof(fromAddr.sin_addr), AF_INET);
	char* srvName = (srvHost != NULL) ? srvHost->h_name : "Sconosciuto";

	    printf("\nStringa %s ricevuta dal server nome: %s indirizzo: %s\n",
	           echoBuffer, srvName, inet_ntoa(fromAddr.sin_addr));

	    // Chiusura
	    closesocket(sock);
	    ClearWinSock();
	    system("pause");
	    return EXIT_SUCCESS;
	}
