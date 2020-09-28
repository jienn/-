#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <netdb.h>

 

void error_handling(char *message);

 

int main(int argc, char *argv[])

{

        int i;

        struct hostent *host;

        if (argc != 2)

        {

               printf("Usage::%s <addr>\n", argv[0]);

               exit(1);

        }

 

        host = gethostbyname(argv[1]); //main 함수를 통해서 전달된 문자열을 인자로 gethostbyname 함수를 호출하고 있다

        if (!host)

               error_handling("gethost.... error");

 

        printf("Official name:%s\n", host->h_name); //공식 도메인 이름을 출력하고 있다

        for (i = 0; host->h_aliases[i]; i++) //공식 도메인 이름 이외의 도메인 이름을 출력하고 있다.

                       printf("Aliases %d:%s\n", i + 1, host->h_aliases[i]);;

        printf("Address tyhpe:%s\n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");

        //IP 주소 정보를 출력하고 있다. 그런데 이해할 수 없는 형변환을 진행하고 있다. 이는 구조체 hostent가 IPv4 뿐만 아니라 IPv6 기반의 주소정보도 저장할 수 있기 때문이다.

        for (i = 0; host->h_addr_list[i]; i++)

               printf("IP addr %d%s\n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));

        return 0;

}

 

void error_handling(char *message)

{

        fputs(message, stderr);

        fputc('\n', stderr);

        exit(1);

}