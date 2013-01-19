#ifndef CLIENT_H
#define CLIENT_H
#include "string.h"
#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
#include "ace/Svc_Handler.h"
#include "ace/Connector.h"
#include "ace/Synch.h"
#include "ace/SOCK_Connector.h"
#include <iostream>
using namespace std;

#define HOST_ADDR "59.69.115.86"

typedef enum msgType {
	REG,ADD,DEL,SAY,ERR,RGR
};

typedef struct Msg {
	enum msgType msg_type;
	char name[16];
	char content[48];
};


class Client;
typedef ACE_Connector<Client,ACE_SOCK_CONNECTOR> Connector;

class Client : public ACE_Svc_Handler<ACE_SOCK_STREAM,ACE_MT_SYNCH>
{
private:
	char name_[16];
	struct Msg *msg_recv_;
	struct Msg *msg_send_;
public:
	Client();
	int regist(char* name);//向服务器发送自己的注册名字
	int open(void*);
	int handle_input(ACE_HANDLE);
	int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask);
	int svc(void);	//该线程负责发送消息
};

#endif
