#pragma comment(lib,"ws2_32") //링커 연결해주기
//#define _WINSOCK_DEPRECATED_NO_WARIINGS
//#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ip2string.h>
#include <WS2tcpip.h>

#define SERVERPORT 9000
#define BUFSIZE 50
void err_quit(char* message);
//사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags){
    int received;
    char* ptr = buf;
    int left = len; //읽어야하는 길이

    while(left>0){ //읽어야하는 크기만큼 반복하기
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
    if(listen_sock == INVALID_SOCKET) err_quit("socket()"); //error_handling()과 같음.

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

    //클라이언트 받을 준비
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE+1];

    while(1){
        //accept() //여러 클라이언트가 접근할 수 있으니까 다른 accept를 받아, 다른 클라이언트를 처리하기 위한 반복문
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if(client_sock == INVALID_SOCKET){
            err_quit("accept()");
            break;
        }

        while(1){ //Half-Close 아직 적용 하지 않았다.
            retval = recvn(client_sock, buf, BUFSIZE,0);
            if(retval == SOCKET_ERROR){
                err_quit("recv()");
                break;
            }
            else if(retval ==0) break; //return value ==0 : 더 이상 받을 메세지가 없다는 뜻. 끝~
            
           
            buf[retval] ='\0'; // 맨끝임을 표시해준다.
            printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr)
                , ntohs(clientaddr.sin_port), buf);
        }
        //closesocket()
        closesocket(client_sock);
        
        printf("[TCPServer] Client CLOSE : IP ADDR = %s, PORT = %d\n"  
            ,inet_ntoa(clientaddr.sin_addr) 
       ,ntohs(clientaddr.sin_port));
    }
    closesocket(listen_sock);
    WSACleanup();

    return 0;
}

void err_quit(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}