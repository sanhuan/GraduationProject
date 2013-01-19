#ifndef SERVER_H
#define SERVER_H
#include "string.h"
#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Svc_Handler.h"
#include "ace/Synch.h"
#include "ace/Thread_Mutex.h"
#include <iostream>
using namespace std;

class Server_Acceptor;
class Server_Stream;
class Accounts;

typedef enum msgType {
	REG,ADD,DEL,SAY,ERR,RGR
};

typedef struct Msg {
	enum msgType msg_type;
	char name[16];
	char content[48];
};

//typedef ACE_Acceptor<Server_Stream,ACE_SOCK_ACCEPTOR> Acceptor;

/*处理client的连接*/
class Server_Acceptor : public ACE_Event_Handler
{
private:
	ACE_SOCK_Acceptor acceptor_;
	Accounts *accounts_;
public:
	Server_Acceptor(ACE_Addr &addr,Accounts *acc);
	int open(ACE_Addr &addr);
	int handle_input(ACE_HANDLE);
	ACE_HANDLE get_handle(void) const
	{
		return acceptor_.get_handle();
	}
};

/*处理client的信息交互*/
class Server_Stream : public ACE_Svc_Handler<ACE_SOCK_STREAM,ACE_MT_SYNCH>
{
private:
	char name_[16];
	Accounts *accounts_;
	bool is_reg;
public:
	Server_Stream(Accounts *acc);
	int open(void*);
	int svc(void);
	int handle_input(ACE_HANDLE);
	int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask);
	void handle_reg(struct Msg* msg);
	void handle_add_server(struct Msg* msg);
	void handle_add_client(struct Msg* msg);
	void handle_del_server(struct Msg* msg);
	void handle_del_client(struct Msg* msg);
	void handle_say_server(struct Msg* msg);
	void handle_say_client(struct Msg* msg);
	void handle_err(struct Msg* msg);
	void handle_rgr(struct Msg* msg);
	int put_message(struct Msg* msg,Server_Stream *ss);
	void send_message(struct Msg* msg);
};

/*client的账户管理*/

typedef struct friendly {
	struct account *p_account;
	struct friendly *next;
};

typedef struct account {
	char name[16];
	Server_Stream *stream;
	struct friendly *p_friend;
	struct account *next;
};

class Accounts
{
private:
	struct account *accounts_;
	int total_;
	ACE_Thread_Mutex mutex_;

public:
	Accounts();
	int add_account(char *name,Server_Stream *ss);
	int dele_account(char *name);
	int add_friend(char *my_name,char *fri_name);
	int dele_friend(char *my_name,char *fri_name);
	int dele_all_friend(char *name);
	struct account* find_account(char *name);
	struct account* find_friend(struct account* acc,char *name);
	Server_Stream* get_stream(char *name);
	void print_account();
	void print_friend(char *name);
};


#endif
