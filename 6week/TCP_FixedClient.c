#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARIINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 50
void err_quit(char* message);

int main(int argc, char* argv[]){
    int retval;

    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa)!=0)
    return 1;

    //socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM,0);
    if(sock == INVALID_SOCKET) err_quit("socket()");

    //connet()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

    if (retval == SOCKET_ERROR) err_quit("connect()");

    char buf[BUFSIZE];
    char* test_data[] = { "English","Blacktea","hungry","die",}; //한글로 하니 오류가 나서 영어로 변경해서 진행함.
    int i = 0;
    for (int i=0;i<4;i++){
        //data input(simulation)
        memset(buf,'#', sizeof(buf)); //버퍼크기만큼 #으로 채우기
        strncpy(buf,test_data[i], strlen(test_data[i])); //#으로 채워진 버퍼 중에서, 문자열 크기만큼 문자열을 복사해와서 덮어쓰기

        //데이터 보내기
        
        retval = send(sock, buf, BUFSIZE,0);
        
        if (retval == SOCKET_ERROR){
            err_quit("send()");
            break;
        }

        printf("[TCP Client] %d BTYE,,, \n", retval);
    }

    closesocket(sock);
    WSACleanup();
    return 0;


}

void err_quit(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}