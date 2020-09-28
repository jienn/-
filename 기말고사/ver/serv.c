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
#define MAX_CLNT 256

#define FILENAMESIZE 256	/* filename field size */
#define FileUpReq    01		/* file upload request */
#define FileDownReq  02		/* file download request */
#define FileAck      11		/* file upload acknowledge */
#define ExitReq		 12 	/* exit request */	

void * HandleFileUpload(int clnt_sock);
void error_handling(char *message);


int main(int argc, char* argv[]){

    //파라미터 개수 검사
    /*
    if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}*/

    in_port_t servPort = PORT; //atoi(argv[1]);

    //Socket() : 서버 소켓 생성
    int serv_sock = socket(PF_INET, SOCK_STREAM,0);
    if(serv_sock == -1) error_handling("socket() error");
    

    //bind : 서버가 사용할 포트/주소를 서버 소켓과 묶음
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET; 
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(servPort);
    
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");
    //listen() : 서버 소켓을 리슨 소켓으로 변경
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

    while(1){
    //accept : 연결후 생성된 클라이언트 소켓을 리턴
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
    if(clnt_sock == -1) error_handling("accept() error");


    //접속한 클라이언트의 정보를 화면에 출력
    printf("connection address : %s\n",inet_ntoa(clnt_addr.sin_addr));
    printf("port NO : %d\n", ntohs(clnt_addr.sin_port));

    while(1){
        //msgType 수신 : 
        uint8_t msgType;
        ssize_t numBytesRcvd = recv(clnt_sock, &msgType, sizeof(msgType),MSG_WAITALL);
        if(numBytesRcvd == -1) error_handling("recv() error");
        
        HandleFileUpload(clnt_sock);

    }
    }
    close(serv_sock);
    return 0;

}

void * HandleFileUpload(int clnt_sock){
    //파일 이름 수신
    char fileName[FILENAMESIZE];
    ssize_t numBytesRcvd = recv(clnt_sock, fileName, FILENAMESIZE, MSG_WAITALL);
    if(numBytesRcvd == -1) error_handling("recv() error");
    //else if(numBytesRcvd == 0) error_handling("file name peer connection closed");
    //else if(numBytesRcvd != FILENAMESIZE) error_handling("file name recv unexpected number of bytes");
    strcat(fileName,"");
    printf("fileName = %s\n",fileName);

    //파일 크기 수신
    uint32_t netFileSize;
    uint32_t fileSize;
    numBytesRcvd = recv(clnt_sock, &netFileSize, sizeof(netFileSize), MSG_WAITALL);
    if(numBytesRcvd == -1) error_handling("recv() error");
    //else if(numBytesRcvd ==0) error_handling("file size peer connection closed");
    //else if(numBytesRcvd != sizeof(netFileSize)) error_handling("file size recv unexpected number of bytes");
    fileSize = ntohl(netFileSize);
    printf("file size : %u\n",fileSize);

    //파일 내용 수신
    FILE *fp = fopen(fileName, "wb");
    if(fp == NULL) error_handling("fopen() error");

    uint32_t rcvdFileSize = 0;
    char fileBuf[BUFSIZE];
    int i =0;
	
    while(rcvdFileSize < fileSize){
        
        //printf("%u\n",ntohl(rcvdFileSize));
        numBytesRcvd = recv(clnt_sock, fileBuf, BUFSIZE,0);
        //f(numBytesRcvd == -1) error_handling("recv() error");
        //else if(numBytesRcvd == 0) error_handling("while recv : peer connection closed");

        fwrite(fileBuf, sizeof(char), numBytesRcvd, fp);
        if(ferror(fp)) error_handling("fwrite() error");

        rcvdFileSize += numBytesRcvd;
    
    }
    fclose(fp);

    //파일 수신 완료

    //파일 수신 성공 메세지(msgType : FileACK)를 클라이언트에게 전송
    uint8_t msgType = FileAck;
    ssize_t numBytesSent = send(clnt_sock,&msgType, sizeof(msgType),0);
    if(numBytesSent == -1) error_handling("send() error");
    else if(numBytesSent != sizeof(msgType)) error_handling("sent unexpected number of bytes");
}

void error_handling (char *message)
{
	fputs (message, stderr);
	fputc ('\n', stderr);
	exit (1);
}