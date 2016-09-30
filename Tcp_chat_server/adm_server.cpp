#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string>
#include <queue>
#include <vector>
#include <map>
using namespace std;
struct client_info{
    struct sockaddr_in _client_addr;
    string _id;
    bool _activated;
};

int main () {
	int sock_fd_server = 0, opts=1;
	struct sockaddr_in server_addr;
	int client_addr_size;

    vector<int> socketid_to_num(100, 0);
    vector<int> num_to_socketid(100, 0);
    vector<queue<string> > msg_queue;
    map<string, int> id_to_num;
    map<int, string> num_to_id;
    vector<client_info> active;
    int total_clients =0;

    char buffer[1024];
    int client_skt[15];
    memset(client_skt, -1, sizeof(client_skt));
    int max_clients = 15;
    struct sockaddr_in client_addr;
    // 서버 소켓 생성 (TDP)
	sock_fd_server = socket(PF_INET, SOCK_STREAM, 0);
    if(setsockopt(sock_fd_server, SOL_SOCKET, SO_REUSEADDR, &opts, sizeof(opts))<0){
        perror("setsockopt error\n");
        return -1;
    }

	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	// TCP/IP 스택의 주소 집합으로 설정
	server_addr.sin_family = AF_INET;
	// 어떤 클라이언트 주소도 접근할 수 있음
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// 서버의 포트를 2000번으로 설정
	server_addr.sin_port = htons(20389);
	// 서버 소켓에 주소 살당
	bind(sock_fd_server, (struct sockaddr*) &server_addr,
		sizeof(struct sockaddr_in));
	// 서버 대기 상태로 설정
	listen(sock_fd_server, 10);
	client_addr_size = sizeof(struct sockaddr_in);
    printf("Waiting for connections ...\n");
    int maxsd;
    // 클라이언트 요청을 수락하고, 클라이언트 소켓 할당
    while(1){
        fd_set readfds;
        FD_ZERO(&readfds);
       // FD_ZERO(&writefds);
        FD_SET(sock_fd_server, &readfds);
        maxsd = sock_fd_server;
        for(int i=0; i<max_clients; ++i){
            int sd  = client_skt[i];
            if(sd>0){
                FD_SET(sd, &readfds);
            }
            if(sd > maxsd){
                maxsd = sd;
            }
        }
        if(select(maxsd+1, &readfds, NULL, NULL, NULL) < 0){
            perror("select() fails\n");
            exit(0);
        };
        //incoming connection
        if(FD_ISSET(sock_fd_server, &readfds)){
            int new_sock_fd_client = accept(sock_fd_server,
		        (struct sockaddr *) &client_addr,
		        (socklen_t *) &client_addr_size);
            if(new_sock_fd_client>0){
                printf("new client #: %d, ", new_sock_fd_client);
                printf("addr: %s, ", inet_ntoa(client_addr.sin_addr));
                printf("port: %u\n", client_addr.sin_port);
                for(int i=0; i<max_clients; ++i){
                    if(client_skt[i] <=0){
                        client_skt[i] = new_sock_fd_client;
                        printf("Adding to list of sockets as client_skt[%d] = %d\n", i, new_sock_fd_client);
                        break;
                    }
                }
            }
        }
        for(int i=0; i<max_clients; ++i){
                int cur_socket = client_skt[i];
                if(cur_socket ==-1){
                    continue;
                }
                if(FD_ISSET(cur_socket, &readfds)){
                    int val_read=read(cur_socket, buffer, 1024);
                    if(val_read <=0){
                        int n = socketid_to_num[cur_socket];
                        printf("socket end cursocket = %d\n", cur_socket);
                        socketid_to_num[cur_socket] = 0;
                        num_to_socketid[n] = 0;
                        active[n]._activated = false;
                        close(cur_socket);
                        client_skt[i] = 0;
                    }else{
                        //normal input
                        if(buffer[0] == 'i'){
                            //login attempt
                            printf("login attempted\n");
                            int l = strlen(buffer);
                            char _id[1024];
                            int j=0;
                            for(int i=4; i<l; ++i){
                                _id[j] = buffer[i];
                                j++;
                            }
                            _id[j] = '\0';
                            int new_sock_fd_client = cur_socket;
                            string id = _id;
                            memset(buffer, '\0', sizeof(buffer));
                            printf("id = %s\n", _id);
                            int num;
                            map<string, int>::iterator it = id_to_num.find(id);
                            if(it == id_to_num.end()){
                                //create new id, and allocate activation number, queue
                                printf("new id! allocate number %d\n", total_clients);
                                id_to_num.insert(make_pair(id, total_clients));
                                num_to_id[total_clients] = id;
                                num = total_clients;
                                queue<string> s;
                                msg_queue.push_back(s);
                                struct client_info p;
                                p._client_addr= client_addr;
                                p._id = id;
                                p._activated = true;
                                active.push_back(p);
                                total_clients++;
                                printf("complete registration!\n");
                            }else{
                                //id exist, so send all messages in the queue
                                printf("existing id, send all messages in the queue\n");
                                num = id_to_num[id];
                                active[num]._activated = true;
                                while(!msg_queue[num].empty()){
                                    string s = msg_queue[num].front();
                                    msg_queue[num].pop();
                                    int len = s.length();
                                    write(new_sock_fd_client, s.c_str(), len);
                                }
                            }
                            printf("login process finished successfully\n");
                            num_to_socketid[num]= new_sock_fd_client;
                            socketid_to_num[new_sock_fd_client] = num;
                        }else{
                            printf("message income\n");
                            int l = strlen(buffer),p=0;
                            for(int i=0; i<l; ++i){
                                 if(buffer[i]=='M' && buffer[i+1] == ':'){
                                     p = i;
                                  }
                             }
                            if(p==0){
                                printf("wrong message form\n");
                                return -1;
                            }
                            char idx[1024], content[1024];
                            int j=0;
                            for(int i=3; i<p-1; ++i){
                                 idx[j] = buffer[i];
                                 j++;
                            }
                            idx[j] = '\0';
                            j=0;
                            for(int i=p+3; i<l; ++i){
                                content[j] = buffer[i];
                                j++;
                            }
                            content[j] = '\0';
                            string sender = num_to_id[socketid_to_num[cur_socket]];
                            string _id = idx;
                            string _msg = content;
                            if(id_to_num.find(_id)==id_to_num.end()){
                                perror("cannot find recipient ID\n");
                                return -1;
                            }
                            _msg = "[Message from " + sender +", Content: " + _msg + "]\n";
                            int num = id_to_num[_id];
                            if(!active[num]._activated){
                                printf("%d client is deactivatd, so keep the msg\n", num);
                                msg_queue[num].push(_msg);
                            }else{
                                int sd = num_to_socketid[num];
                                write(sd, _msg.c_str(), _msg.length());
                            }
                        }
                    }
                    memset(buffer, '\0', sizeof(buffer));
           }
        }
    }
    // 서버 소켓 종료
	close(sock_fd_server);
	return 0;
}
