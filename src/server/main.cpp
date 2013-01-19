#include "server.h"

int main(int argc,char* argv[])
{

	ACE_INET_Addr addr(8000);	//端口地址为8000
	
	Accounts *accounts = new Accounts();
	
	Server_Acceptor *sa = new Server_Acceptor(addr,accounts);
	
	
	ACE_Reactor::instance()->register_handler(sa,ACE_Event_Handler::ACCEPT_MASK);
	
	while(1)
		ACE_Reactor::instance()->handle_events();


#if 0
	Accounts *accounts = new Accounts();
	char *name1 = new char[16];
	char *name2 = new char[16];
	char *name3 = new char[16];
	strcpy(name1,"peng");
	strcpy(name2,"xiao");
	strcpy(name3,"dong");
	
	accounts->add_account(name1,NULL);
	accounts->add_account(name2,NULL);
	accounts->add_account(name3,NULL);
	accounts->print_account();
	
	accounts->add_friend(name1,name2);
	accounts->add_friend(name1,name3);
	accounts->add_friend(name2,name1);
	accounts->add_friend(name3,name1);
	accounts->add_friend(name2,name3);
	accounts->add_friend(name3,name2);
	accounts->print_friend(name1);

	accounts->dele_account(name1);
	//accounts->dele_all_friend(name1);
	accounts->add_account(name1,NULL);
	accounts->add_friend(name1,name2);
	accounts->add_friend(name1,name3);
	accounts->add_friend(name2,name1);
	accounts->add_friend(name3,name1);
	
	
	accounts->print_friend(name1);
	accounts->print_friend(name2);
	accounts->print_friend(name3);
#endif
	
	return 0;
}
