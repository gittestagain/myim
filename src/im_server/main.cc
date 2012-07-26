#include <common/def.h>
#include <common/ipc.h>
#include <iostream>
using namespace std;
#include <vector>  
#include <winsock2.h>  
#pragma comment(lib, "ws2_32.lib") 
#define BUF_SIZE 15


using namespace ipc;

enum {
	MSG_TYPE_LOGIN = 1,
};

struct msg_head_t
{
	byte type_;
};

struct msg_login_t : public msg_head_t
{
	char name_[32];
};

class client_link_t : public u_thread
{
public:
	virtual void loop(void)
	{
		/*int len = 0;
		byte *msg = recv(len);

		msg_head_t *mh = (msg_head_t*)msg;

		switch (mh->type_)
		{
		case MSG_TYPE_LOGIN:
			if (len == sizeof(msg_login_t)) {
				msg_login_t *ml = (msg_login_t*)mh;
				ml->name_;
			}

			break;
		}*/
		SOCKET          sServer;        //�������׽���  
		SOCKET          sClient;        //�ͻ����׽���  
		int             retVal;         //����ֵ 
		char            buf[BUF_SIZE];  //�������ݻ�����  
		char            sendBuf[BUF_SIZE];//���ظ��ͻ��˵�����  
		ZeroMemory(buf, BUF_SIZE);  
		retVal = recv(sClient, buf, BUF_SIZE, 0);                     

		if (-1 == retVal)  
		{  
			cout << "recv failed!" << endl;       
			closesocket(sServer);   //�ر��׽���  
			closesocket(sClient);   //�ر��׽���       
			WSACleanup();           //�ͷ��׽�����Դ;  
		//	return -1;  
		}  
		/*if(buf[0] == '0')  
			break;*/  
		buf[retVal]='\0';
		cout << "�ͻ��˷��͵�����: " << buf<<endl ; 

		//��ͻ��˷������� 
		cout << "��ͻ��˷�������: " ;  
		cin >> sendBuf;  

		send(sClient, sendBuf, strlen(sendBuf), 0);  
}

	client_link_t(int sock)
	{
		sock_ = sock;
	}

	int sock_;
};

class link_manager_t
{
public:
	bool add_link(int sock)
	{
		if(sock >= 0)
		{
			
		}
	}
//	void del_link;
	bool broadcast(char *msg, int len);

	std::vector<client_link_t*>  links_;
};

int main(void)
{
	WSADATA         wsd;            //WSADATA����  
	SOCKET          sServer;        //�������׽���  
	SOCKET          sClient;        //�ͻ����׽���  
	SOCKADDR_IN     addrServ;;      //��������ַ  
	char            buf[BUF_SIZE];  //�������ݻ�����  
	char            sendBuf[BUF_SIZE];//���ظ��ͻ��˵�����  
	int             retVal;         //����ֵ  
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)  
	{  
		cout << "WSAStartup failed!" << endl;  
		return 1;  
	}  

	//�����׽���  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);      
	if(0 > sServer)  
	{  
		cout << "socket failed!" << endl;  
		WSACleanup();//�ͷ��׽�����Դ;  
		return  -1;  
	}  

	//�������׽��ֵ�ַ   
	addrServ.sin_family = AF_INET;  
	addrServ.sin_port = htons(4999);  
	addrServ.sin_addr.s_addr = INADDR_ANY;        
	//���׽���  
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));  

	if(-1 == retVal)  
	{     
		cout << "bind failed!" << endl;  
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;  
	} 

	retVal = listen(sServer, 10);  
	if(-1 == retVal)  
	{  
		cout << "listen failed!" << endl;         
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}


	sockaddr_in addrClient;  
	int addrClientlen = sizeof(addrClient);  
	sClient = accept(sServer,(sockaddr FAR*)&addrClient, &addrClientlen);  
	if(0 > sClient)  
	{  
		cout << "accept failed!" << endl;         
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;  
	}  
	//�˳�  
	closesocket(sServer);   //�ر��׽���  
	closesocket(sClient);   //�ر��׽���  
	WSACleanup();           //�ͷ��׽�����Դ;
	return 0;
}