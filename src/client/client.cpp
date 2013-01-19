#include "client.h"

Client::Client()
{
	msg_recv_ = new struct Msg;
	msg_send_ = new struct Msg;
}

int Client::open(void*)
{
	ACE_OS::printf("Connection established!\n");
	
	ACE_Reactor::instance()->register_handler(this,ACE_Event_Handler::READ_MASK);
	if(activate(THR_NEW_LWP,1) == -1)
		ACE_OS::printf("Create thread failed!\n");
			
	return 0;
}

int Client::regist(char* name)
{
	//向服务器发送自己的注册名字
	strcpy(name_,name);
	msg_send_->msg_type = REG;
	strcpy(msg_send_->name,name_);
	
	int cnt = peer().send_n(msg_send_,sizeof(struct Msg));
	
	if(-1 == cnt || 0 == cnt)
	{
		ACE_OS::printf("Server has not open!\n");
		exit(-1);
	}
	
	return 0;
}

int Client::handle_close(ACE_HANDLE,ACE_Reactor_Mask)
{
	delete(msg_recv_);
	delete(msg_send_);
	ACE_OS::printf("shut down!\n");
	return 0;
}

int Client::handle_input(ACE_HANDLE)
{
	int count = peer().recv(msg_recv_,sizeof(struct Msg));
	
	if(-1 == count)
	{
		ACE_OS::printf("peer receive failed!\n");
		return -1;
	}
	else if(0 == count)
	{
		ACE_OS::printf("peer receive shut down!\n");
		return -1;
	}
	else
	{
		if(msg_recv_->msg_type == REG)
			ACE_OS::printf("Regist success!\n");
		else if(msg_recv_->msg_type == ADD)
			ACE_OS::printf("%s %s\n",msg_recv_->name,msg_recv_->content);
		else if(msg_recv_->msg_type == DEL)
			ACE_OS::printf("%s %s\n",msg_recv_->name,msg_recv_->content);
		else if(msg_recv_->msg_type == SAY)
			ACE_OS::printf("%s says : %s\n",msg_recv_->name,msg_recv_->content);
		else if(msg_recv_->msg_type == ERR)
			ACE_OS::printf("%s\n",msg_recv_->content);
		else if(msg_recv_->msg_type == RGR)
		{
			ACE_OS::printf("Your name has been registed!\n");
			exit(-1);
		}
		return 0;
	}
}

int Client::svc(void)
{
	char buf_in[67];
	char buf_cmd[3],buf_name[16],buf_content[48];
	while(1)
	{
		int cnt;
		cin.getline(buf_in,67);
		sscanf(buf_in,"%s %s %[^\n]",buf_cmd,buf_name,buf_content);
		if(0 == strcasecmp(buf_cmd,"ADD"))
		{
			msg_send_->msg_type = ADD;
			strcpy(msg_send_->name,buf_name);
		}
		else if(0 == strcasecmp(buf_cmd,"DEL"))
		{
			msg_send_->msg_type = DEL;
			strcpy(msg_send_->name,buf_name);
		}
		else if(0 == strcasecmp(buf_cmd,"SAY"))
		{
			msg_send_->msg_type = SAY;
			strcpy(msg_send_->name,buf_name);
			strcpy(msg_send_->content,buf_content);
		}
		else if(0 == strcasecmp(buf_cmd,"REG"))
		{
			ACE_OS::printf("You have registed!\n");
			continue;
		}
		else
		{
		 	ACE_OS::printf("Your form is wrong,please input again!\n");
		 	continue;
		}
		
		cnt = peer().send_n(msg_send_,sizeof(struct Msg));
		
		if(-1 == cnt)
		{
			ACE_OS::printf("peer send failed!\n");
			exit(-1);
		}
		else if(0 == cnt)
		{
			ACE_OS:printf("peer shut down!\n");
			exit(-1);
		}
		else
			ACE_OS::printf("peer send %d datas\n",cnt);
	}

	
	return 0;
}
