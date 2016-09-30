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
	// ������ ���� �ּ� ������ ���� ����ü
	struct sockaddr_in server_addr;
	// �������� �޾ƿ� ������ ���� ����
	char message[1024];
	// ���� ������ ���� ����(TCP) ����
	sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    if(sock_fd ==-1){
        printf("couldn't create the socket");
        return 0;
    }
    // �ʱ�ȭ
	memset(&server_addr, 0, sizeof(server_addr));
	// TCP/IP ������ ���� �ּ� ����(Address Family)���� ����
	server_addr.sin_family = AF_INET;
	// ������ ������ �ּҴ� ���� ȣ��Ʈ(127.0.0.1)�� ����
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// ������ ������ ��Ʈ ��ȣ�� 2000��
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
// ���� ����
	return 0;
}
