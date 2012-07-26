#include <common/def.h>
#include "winsock2.h"  
#include <iostream>
using namespace std;
#pragma comment(lib, "ws2_32.lib") 

BOOL    RecvLine(SOCKET s, char* buf);  //��ȡһ������  

int main()
{

	const int BUF_SIZE = 15;  

	WSADATA         wsd;            //WSADATA����  
	SOCKET          sHost;          //�������׽���  
	SOCKADDR_IN     servAddr;       //��������ַ  
	char            buf[BUF_SIZE];  //�������ݻ�����  
	char            bufRecv[BUF_SIZE];  
	int             retVal;         //����ֵ  

	//��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)  
	{  
		cout << "WSAStartup failed!" << endl;  
		return -1;  
	}  

	//�����׽���  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
	if(INVALID_SOCKET == sHost)  
	{  
		cout << "socket failed!" << endl;  
		WSACleanup();//�ͷ��׽�����Դ  
		return  -1;  
	}  

	//���÷�������ַ  
	servAddr.sin_family =AF_INET;  
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
	servAddr.sin_port = htons((short)4999);  
	int nServAddlen  = sizeof(servAddr);  

	//���ӷ�����  
	retVal=connect(sHost,(LPSOCKADDR)&servAddr, sizeof(servAddr));    
	if(SOCKET_ERROR == retVal)  
	{  
		cout << "connect failed!" << endl;    
		closesocket(sHost); //�ر��׽���  
		WSACleanup();       //�ͷ��׽�����Դ  
		return -1;  
	}  
	while(true){  
		//���������������  
		ZeroMemory(buf, BUF_SIZE);  
		cout << " ���������������:  ";  
		cin >> buf;  
		retVal = send(sHost, buf, strlen(buf), 0);  
		if (SOCKET_ERROR == retVal)  
		{  
			cout << "send failed!" << endl;  
			closesocket(sHost); //�ر��׽���  
			WSACleanup();       //�ͷ��׽�����Դ  
			return -1;  
		} 

		//�ӷ�������������
		recv(sHost, bufRecv,15 , 0);// ���շ������˵����ݣ� ֻ����15���ַ�  
		//bufRecv[BUF_SIZE]='\0';
		cout << endl <<"�ӷ������������ݣ�" << bufRecv;  

	}  
	//�˳�  
	closesocket(sHost); //�ر��׽���  
	WSACleanup();       //�ͷ��׽�����Դ  
	return 0;  
}