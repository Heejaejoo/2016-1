#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
using namespace std;

int main () {

    fd_set fs_status;
    int sock_fd = 0;
	// 연결할 서버 주소 정보를 위한 구조체
	struct sockaddr_in server_addr;
	// 서버에서 받아올 데이터 저장 공간
	char message[1024];
	// 서버 접속을 위한 소켓(TCP) 생성
	sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    if(sock_fd ==-1){
        printf("couldn't create the socket");
        return 0;
    }
    // 초기화
	memset(&server_addr, 0, sizeof(server_addr));
	// TCP/IP 스택을 위한 주소 집합(Address Family)으로 설정
	server_addr.sin_family = AF_INET;
	// 연결할 서버의 주소는 로컬 호스트(127.0.0.1)로 지정
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// 연결할 서버의 포트 번호는 2000번
	server_addr.sin_port = htons(20389);
	if(connect(sock_fd, (struct sockaddr*) &server_addr,
		sizeof(server_addr))<0){
        printf("failed to connect to server, try again");
        return 0;
    };
    printf("## Connected Successfully!\n");
   printf("## To deactivate, type 'quit' after login\n");
    printf("## Must follow the id form without parenthetis [[id: ~~]]\n");
    printf("## Also follow the message form [[R: recipient M: message]]\n");
    printf("## Message should be no longer than 1000bytes\n");
    printf("\n## First, please type in your id for login\n");
    while(1){
        FD_ZERO(&fs_status);
        FD_SET(0, &fs_status);
        FD_SET(sock_fd, &fs_status);
        select(sock_fd+1, &fs_status, NULL, NULL, NULL);
        if(FD_ISSET(0, &fs_status)){
            //keyboard input
            read(0, message, 1024);
            message[strlen(message)-1] = '\0';
            if('q' == message[0]){
                close(sock_fd);
                return 0;
            }else if ('i' == message[0]){
               if(write(sock_fd, message, sizeof(message))<0){
                    printf("something wrong on login\n");
                    close(sock_fd);
                    return -1;
               }
               printf("## Login success\n## Enjoy chat\n");
                }else{
                if(write(sock_fd, message, sizeof(message))<0){
                    printf("something wrong on send message\n");
                    close(sock_fd);
                    return -1;
                }
            }
            memset(message, '\0', sizeof(message));

        } else if (FD_ISSET(sock_fd, &fs_status)){
            int rd = read(sock_fd, message, 1024);
            if(rd <0){
                perror("cannot read from server");
                return 0;
            }
            message[rd] = '\0';
            printf("%s", message);
            memset(message, '\0', sizeof(message));
        }
    }
// 연결 종료
	return 0;
}
