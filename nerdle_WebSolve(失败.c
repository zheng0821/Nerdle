#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main(){
    //��ʼ��DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2),&wsaData);
    
    //�����׽���
    SOCKET sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock!=INVALID_SOCKET){
        printf("�׽��ִ����ɹ�\n");
    }
    //���׽���
    SOCKADDR_IN server;
    ZeroMemory(&server,sizeof(server));
    server.sin_family=PF_INET;
    server.sin_addr.s_addr=inet_addr("13.225.4.54");//nerdlegame.com 
    server.sin_port=htons(80);
    //���ӵ�������
    connect(sock,(SOCKADDR*)&server,sizeof(server));

    char *message = "GET / HTTP/1.1\r\nHost: nerdlegame.com\r\nConnection: close\r\n\r\n";
    send(sock,message,strlen(message),0);

    char buffer[1024];
    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        printf("failed");
    // ������յ�������
    }else{
    	printf("%s",buffer);
	}
    closesocket(sock);
    //ֹͣʹ��DLL
    WSACleanup();
    return 0;
}
