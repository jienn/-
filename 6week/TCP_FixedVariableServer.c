#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9190
#define BUFSIZE 512
void err_quit(char* message);
//사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags){
    int received;
    char* ptr = buf;
    int left = len;

    while(left>0){
        received = recv(s, ptr, left, flags);
        if(received == SOCKET_ERROR)
        return SOCKET_ERROR;
        else if(received == 0)
        break;

        left -= received;
        ptr += received;
    }

    return (len-left);
}

int main(int argc, char* argv[]){
    int retval;

    //윈속 초기화
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa)!=0)
    return 1;

    //socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock == INVALID_SOCKET) err_quit("socket()");

    //bind
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if(retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if(retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen,len;
    char buf[BUFSIZE+1];

    while(1){
        //accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if(client_sock == INVALID_SOCKET){
            err_quit("accept()");
            break;
        }

        //printf("PORT = %d\n", SERVERPORT);
        while(1){ //데이터 크기의 값과 데이터를 받기에 recvn을 2번 진행함.
            int len = 0; //길이 받을 변수 선언
            //데이터 받기(고정길이)
            retval = recvn(client_sock, (char *)&len, sizeof(int), 0); //데이터의 길이를 int형으로 받음
            if (retval == SOCKET_ERROR) {
                err_quit("recv()");
                break;
            }
            else if (retval == 0) break;

            //printf("len = %d\n", len);
            // 데이터 받기(가변길이_데이터)
            retval = recvn(client_sock, buf, len, 0);
            if(retval == SOCKET_ERROR) {
                err_quit("recv()"); break;
            }
            else if (retval ==0) break;

            //받은 데이터 출력
            buf[retval] ='\0'; //맨 끝임을 표시
            printf("[TCP/%s:%d] %s\n", 
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
        }
        //closesocket()
        closesocket(client_sock);
        printf("[TCPServer] Client CLOSE : IP ADDR = %s, PORT = %d\n"
            , inet_ntoa(clientaddr.sin_addr)
            , ntohs(clientaddr.sin_port));
    }
    closesocket(listen_sock); //소켓 종료
    WSACleanup(); //윈속 해제

    return 0;
}

void err_quit(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}