#include "server.h"

Server_Stream::Server_Stream(Accounts *acc)
{
	accounts_ = acc;
	is_reg = false;
	open(0);
}

int Server_Stream::open(void*)
{
	activate(THR_NEW_LWP,1);
	ACE_OS::printf("Server_Stream opened!\n");
	return 0;
}

/*接收消息队列中的消息块*/
int Server_Stream::svc(void)
{
	while(1)
	{
		ACE_Message_Block *mb;
		if(-1 == this->getq(mb))
		{
			ACE_OS::printf("%s receiv block failed!\n",name_);
			mb->release();
			return 0;
		}
		else
			ACE_OS::printf("%s receiv block!\n",name_);

		struct Msg *msg = (struct Msg *)mb->rd_ptr();
		
		if(ADD == msg->msg_type)
			handle_add_server(msg);
		else if(DEL == msg->msg_type)
			handle_del_server(msg);
		else if(SAY == msg->msg_type)
			handle_say_server(msg);
	
		mb->release();
	}
	return 0;
}

int Server_Stream::handle_input(ACE_HANDLE)
{
	struct Msg *msg = new struct Msg;
	int cnt = peer().recv(msg,sizeof(struct Msg));
	if(-1 == cnt)
	{
		ACE_OS::printf("Receive failed!\n");
		return -1;
	}
	else if(0 == cnt)
	{
		ACE_OS::printf("Shut down!\n");
		return -1;
	}
	
	if(REG == msg->msg_type)
		handle_reg(msg);
	else if(ADD == msg->msg_type)
		handle_add_client(msg);
	else if(DEL == msg->msg_type)
		handle_del_client(msg);
	else if(SAY == msg->msg_type)
		handle_say_client(msg);
		
	return 0;
}

/*该事件退出时被调用*/
int Server_Stream::handle_close(ACE_HANDLE,ACE_Reactor_Mask)
{
	if(!is_reg)
		return 0;
	if(0 == accounts_->dele_account(name_))
		ACE_OS::printf("Handle close erro!\n");
	else
		ACE_OS::printf("Shut down!\nHandle_close\n");
	return 0;
}

void Server_Stream::handle_reg(struct Msg *msg)
{
	char *name = msg->name;
	struct Msg *_msg = new struct Msg;
	if(accounts_->add_account(name,this))
	{
		strcpy(name_,name);
		is_reg = true;
		_msg->msg_type = REG;
		send_message(_msg);
		ACE_OS::printf("Add account %s\n",name);
	}
	else
	{
		_msg->msg_type = RGR;
		send_message(_msg);
	}
}

void Server_Stream::handle_add_server(struct Msg *msg)
{
	if(NULL != accounts_->find_friend(accounts_->find_account(name_),msg->name))
	{
		strcpy(msg->content,"add you, you can talk together!");
		send_message(msg);
	}
	else
	{
		strcpy(msg->content,"add you, you have not added it!");
		send_message(msg);
	}
}

void Server_Stream::handle_add_client(struct Msg *msg)
{
	int n = accounts_->add_friend(name_,msg->name);
	if(0 == n)	//添加成功
	{
		Server_Stream *ss = accounts_->get_stream(msg->name);
		msg->msg_type = ADD;
		strcpy(msg->name,name_);
		put_message(msg,ss);
	}
	else if(1 == n)	//添加失败
	{
		msg->msg_type = ERR;
		strcpy(msg->content,"is not exist!");
		send_message(msg);
	}
	else if(2 == n)	//添加失败
	{
		msg->msg_type = ERR;
		strcpy(msg->content,"has been in your list!");
		send_message(msg);
	}
	else
		exit(-1);
}

void Server_Stream::handle_del_server(struct Msg *msg)
{
	if(NULL != accounts_->find_friend(accounts_->find_account(name_),msg->name))
	{
		strcpy(msg->content,"delete you, you have not delete it!");
		send_message(msg);
	}
	else
	{
		strcpy(msg->content,"delete you, you are broken!");
		send_message(msg);
	}
}

void Server_Stream::handle_del_client(struct Msg *msg)
{
	if(accounts_->dele_friend(name_,msg->name))
	{
		msg->msg_type = ERR;
		strcpy(msg->content,"is not in your list!");
		send_message(msg);
	}
	else
	{
		Server_Stream *ss = accounts_->get_stream(msg->name);
		msg->msg_type = DEL;
		strcpy(msg->name,name_);
		put_message(msg,ss);
	}
}

void Server_Stream::handle_say_server(struct Msg *msg)
{
	send_message(msg);
}

void Server_Stream::handle_say_client(struct Msg *msg)
{
	char *name = msg->name;
	struct account *acc1 = accounts_->find_account(name_);
	struct account *acc2 = accounts_->find_account(name);
	if(NULL == acc2)
	{
		msg->msg_type = ERR;
		strcpy(msg->content,"The friend is not exit!");
		send_message(msg);
		return;
	}
	if(NULL == accounts_->find_friend(acc1,name))
	{
		msg->msg_type = ERR;
		strcpy(msg->content,"You have not add the friend!");
		send_message(msg);
		return;
	}
	if(NULL == accounts_->find_friend(acc2,name_))
	{
		msg->msg_type = ERR;
		strcpy(msg->content,"The friend has not add you!");
		send_message(msg);
		return;
	}
	
	strcpy(msg->name,name_);
	put_message(msg,acc2->stream);
}

void Server_Stream::handle_err(struct Msg *msg)
{
	return;
}

void Server_Stream::handle_rgr(struct Msg *msg)
{
	return;
}

int Server_Stream::put_message(struct Msg *msg,Server_Stream *ss)
{
	if(NULL == ss)
		return -1;
	
	ACE_Message_Block *mb;
	ACE_NEW_RETURN(mb,ACE_Message_Block(sizeof(struct Msg),		//构造消息块
					ACE_Message_Block::MB_DATA,
					0,
					(char *)msg),0);
					
	mb->wr_ptr(sizeof(struct Msg));	//设置消息块的写指针
		
	if(-1 == ss->putq(mb))	//将消息块加入对点端对象的队列
		ACE_OS::printf("%s put queue failed!\n",name_);
	else
		ACE_OS::printf("%s put queue\n",name_);
	
	return 0;
}

void Server_Stream::send_message(struct Msg *msg)
{
	int cnt = peer().send_n(msg,sizeof(struct Msg));
	if(-1 == cnt)
	{
		ACE_OS::printf("%s send failed!\n",name_);
		return;
	}
	else if(0 == cnt)
	{
		ACE_OS::printf("%s shut down!\n",name_);
		return;
	}
	else
		ACE_OS::printf("%s send %d datas\n",name_,cnt);
}













