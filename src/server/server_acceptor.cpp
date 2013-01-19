#include "server.h"

Server_Acceptor::Server_Acceptor(ACE_Addr &addr,Accounts *acc)
{
	open(addr);
	accounts_ = acc;
}

int Server_Acceptor::open(ACE_Addr &addr)
{
	if(acceptor_.open(addr) == -1)
	{
		ACE_OS::printf("Server_Acceptor opened failed!\n");
		ACE_OS::exit(1);
	}
	else
		ACE_OS::printf("Server_Acceptor opened!\n");
	return 0;
}

int Server_Acceptor::handle_input(ACE_HANDLE)
{
	Server_Stream *ss = new Server_Stream(accounts_);
	
	/*接收连接*/
	if(acceptor_.accept(
					ss->peer(),
					0,
					0,
					1) == -1)
	{
		ACE_OS::printf("Error in connect!\n");
		return 0;
	}
	
	ACE_OS::printf("Connection establidhed!\n");
	
	/*注册READ_MASK事件*/
	ACE_Reactor::instance()->
		register_handler(ss,ACE_Event_Handler::READ_MASK);
	
	return 0;
}





