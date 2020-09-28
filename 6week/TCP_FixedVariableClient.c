#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARIINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9190
#define BUFSIZE 50
void err_quit(char* message);

int main(int argc, char* argv[]) {
    int retval;

   //윈속 초기화 
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    //socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    //connet()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

    if (retval == SOCKET_ERROR) err_quit("connect()");

    char buf[BUFSIZE];
    char* test_data[] = { "English","Blacktea","hungry","die", };
    int i = 0;

    int len;

    for (int i = 0;i < 4;i++) { //테스트 데이터가 4개라서 4번 반복하는 반복문
        //데이터의 길이와 데이터를 보내기에 두번 send를 진행한다.
        //데이터 입력(시뮬레이션)
        len = strlen(test_data[i]); //각 테스트데이터의 원소마다 크기를 저장한다.
        memset(buf, 0, sizeof(buf)); //버퍼를 초기화시킨다.
        strncpy(buf, test_data[i], len); //테스트데이터의 문자열크기만큼 문자열을 buf에 덮어쓴다.

        //데이터 보내기(고정길이)

        retval = send(sock, (char*)&len, sizeof(int), 0); //데이터의 길이를 len에 저장한다.

        if (retval == SOCKET_ERROR) {
            err_quit("send1()");
            break;
        }

        //데이터 보내기(가변길이)

        retval = send(sock, buf, len, 0); //데이터의 내용을 buf에 저장한다.

        if (retval == SOCKET_ERROR) {
            err_quit("send2()");
            break;
        }

        printf("[TCP Client] %d+%d BTYE,,, \n", sizeof(int), retval);
    }

    closesocket(sock); //소켓 종료
    WSACleanup(); //윈속 초기화
    return 0;


}

void err_quit(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}