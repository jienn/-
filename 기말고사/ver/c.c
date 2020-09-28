#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFSIZE 4096
#define PORT 8000
#define IP "127.0.0.1";

#define FILENAMESIZE 256	/* filename field size */
#define FileUpReq    01		/* file upload request */
#define FileDownReq  02		/* file download request */
#define FileAck      11		/* file upload acknowledge */
#define ExitReq		 12 	/* exit request */	
#define MAX_CLNT 256
#define FILENAMESIZE 256	/* filename field size */
#define FileUpReq    01		/* file upload request */
#define FileDownReq  02		/* file download request */
#define FileAck      11		/* file upload acknowledge */
#define ExitReq		 12 	/* exit request */	

void FileUploadProcess(int sock, char* A_fileName);
void error_handling(char *message);
void* thread_main(void *arg);

int main(int argc, char *argv[]) 
{
	pthread_t t_id;
	int thread_param=5;
	void * thr_ret;
	
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread_create() error");
		return -1;
	}; 	

	if(pthread_join(t_id, &thr_ret)!=0)
	{
		puts("pthread_join() error");
		return -1;
	};

	free(thr_ret);
	return 0;
}

void* thread_main(void *arg) 
{

	//명령 파라미터 개수가 검사
	/*
	if (argc!=3){
		printf("Usage : %s <Server Address> <server Port>\n", argv[0]);
		exit(1);
	}
	*/

	char* servIP = IP;//argv[1];
	char operand[50];
	in_port_t servPort = PORT;//atoi(argv[2]);

	//소켓 생성
	int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1)
	error_handling("socket() error");

	//connect : 서버에 접속
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(servIP);
	serv_addr.sin_port=htons(servPort);
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");

	printf("파일이름(확장자포함) : ");
	scanf("%s",operand);
	FileUploadProcess(sock, operand);

	shutdown(sock, SHUT_WR);
	close(sock);
	return 0;



}


void FileUploadProcess(int sock, char* A_fileName){

	//msgType 전송 : 필드크기는 1Bytes로 고정
	uint8_t msgType = FileUpReq;
	size_t numBytesSent = send(sock, &msgType,sizeof(msgType),0);
	if(numBytesSent == -1) error_handling("send() error");
	else if(numBytesSent != sizeof(msgType)) error_handling("msgType sent unexpected number of bytes");

	//파일 이름을 서버에 전송 : 필드크기를 256 바이트로 고정
	char fileName[FILENAMESIZE];
	memset(fileName,0,FILENAMESIZE);
	strcpy(fileName, A_fileName);
	numBytesSent = send(sock, fileName, FILENAMESIZE, 0);
	if(numBytesSent == -1 ) error_handling("send() error");
	else if(numBytesSent != FILENAMESIZE) error_handling("fileName sent unexpected number of bytes");

	//파일크기 전송
	struct stat sb;
	if(stat(fileName, &sb)<0)
	error_handling("stat() error");

	uint32_t fileSize = sb.st_size;
	uint32_t netFileSize = htonl(fileSize);
	numBytesSent = send(sock, &netFileSize, sizeof(netFileSize),0);
	if(numBytesSent == -1 ) error_handling("send() error");
	else if(numBytesSent != sizeof(netFileSize)) error_handling("file size sent unexpected number of bytes");

	//파일 내용 전송
	FILE *fp = fopen(fileName,"rb"); //바이너리모드
	if(fp == NULL) error_handling("fopen() error");

	while(!feof(fp)){
		char fileBuf[BUFSIZE];
		size_t numBytesRead = fread(fileBuf, sizeof(char), BUFSIZE, fp);
		if(ferror(fp)) error_handling("fread() error");

		numBytesSent = send(sock, fileBuf, numBytesRead,0);
		if(numBytesSent == -1 ) error_handling("send() error");
		else if(numBytesSent != numBytesRead) error_handling("file sent unexpected numver of bytes");

	}
	fclose(fp);

	//서버로부터 ACK 메세지 수신 후 화면에 성공여부 출력
	ssize_t numBytesRcvd = recv(sock, &msgType, sizeof(msgType),MSG_WAITALL);
	if(numBytesRcvd == -1) error_handling("recv() error");
	else if(numBytesRcvd == 0) error_handling("peer connection closed");
	else if(numBytesRcvd != sizeof(msgType)) error_handling("recv unexpected number of bytes");

	if(msgType == FileAck)
		printf("file transmit 성공 : %u Bytes\n", fileSize);
	else
		printf("file transmit 실패\n");
	

}

void error_handling (char *message)
{
	fputs (message, stderr);
	fputc ('\n', stderr);
	exit (1);
}

