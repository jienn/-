#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4
void error_handling(char *message);
void end_flag(char *flag);

int main(int argc, char *argv[])
{
	int sock;
	//***MY CODE
	char message[BUF_SIZE];
	char empty;
	int str_len;
	//
	char opmsg[BUF_SIZE];
	int result, i, opnd_cnt=0;
	struct sockaddr_in serv_adr;
	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");

	while(1){
	fputs("Operand count (q to quit) : ", stdout);
	fgets(message, BUF_SIZE, stdin);	

	if(!strcmp(message,"q\n"))
	{	printf("flag is q\n\n"); 
		opnd_cnt=0; opmsg[0]=opnd_cnt;
		write(sock, opmsg, opnd_cnt*OPSZ+2);
		break;	
	}
	
	opnd_cnt = atoi(message); //ascii로 입력된 숫자를 int형 정수로 변환

	printf(">>>>>>>opnd_cnt is %d\n", opnd_cnt);
	opmsg[0]=opnd_cnt;
	printf("opmsg[0] == %c. %d\n\n", opmsg[0],opmsg[0]);
	
	if(strcmp(message,"q\n")){
	for(i=0; i<opnd_cnt; i++)
	{
		printf("Operand %d: ", i+1);
		scanf("%d",(int*)&opmsg[i*OPSZ+1] );
		//empty = fgetc(stdin);
		//opmsg[i*OPSZ+1] = (int)empty - '0';
	}
	fgetc(stdin);
	printf("Operator: ");
	scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
	//fgets(&empty, sizeof(empty), stdin);
	//opmsg[opnd_cnt*OPSZ+1] = empty;

	write(sock, opmsg, opnd_cnt*OPSZ+2);
	printf("write success!!!\n");
	
	read(sock, &result, RLT_SIZE);
	printf("Operation result: %d \n", result);
	fgetc(stdin);
	}

	}
	close(sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void end_flag(char *flag){
	
	

}