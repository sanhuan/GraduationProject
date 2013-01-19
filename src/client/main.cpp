#include "client.h"

int main(int argc,char *argv[])
{
	char buf[16];
	ACE_INET_Addr addr(8000,HOST_ADDR);
	
	if(argc != 2)
	{
		ACE_OS::printf("%s <name>\n",argv[0]);
		exit(0);
	}
	strcpy(buf,argv[1]);
	Client *client = new Client();
	
	Connector connector;
	if(connector.connect(client,addr) == -1)
	{
		ACE_OS::printf("Connect failed!");
		exit(0);
	}
	
	client->regist(buf);	//注册名字
	
	while(1)
		ACE_Reactor::instance()->handle_events();

	return 0;
}
