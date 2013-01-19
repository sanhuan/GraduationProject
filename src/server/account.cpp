#include "server.h"

Accounts::Accounts()
{
	total_ = 0;
	accounts_ = new struct account;
	accounts_->stream = NULL;
	accounts_->p_friend = NULL;
	accounts_->next = NULL;
}

int Accounts::add_account(char *name,Server_Stream *ss)
{
	if(NULL != find_account(name))	//若帐号已经存在，则直接返回0
		return 0;
	mutex_.acquire();	//加锁
	struct account *acc = accounts_;
	while(NULL != acc->next)	//到链表末尾
		acc = acc->next;
	acc->next = new struct account;
	acc = acc->next;
	strcpy(acc->name,name);
	acc->stream = ss;
	acc->p_friend = new struct friendly;
	acc->p_friend->p_account = NULL;
	acc->p_friend->next = NULL;
	acc->next = NULL;
	total_++;
	ACE_OS::printf("Add account %s\n",acc->name);
	mutex_.release();	//解锁
	
	return 1;	//加入成功，返回1
}

int Accounts::dele_account(char *name)
{
	if(0 == total_)
		return 0;
	//加锁
	struct account *acc_front = accounts_;
	struct account *acc_back = accounts_->next;
	for(int i=0;i<total_;i++)
	{
		if(0 == strcmp(acc_back->name,name))
		{
			//释放帐号的相关信息所占空间
			dele_all_friend(name);
			//释放该帐号
			acc_front->next = acc_back->next;
			mutex_.acquire();
			delete(acc_back->p_friend);
			delete(acc_back);
			total_--;
			mutex_.release();
			return 1;
		}
		else
		{
			acc_front = acc_back;
			acc_back = acc_back->next;
		}
	}

	return 0;	//没有找到需要删除的帐号
}

int Accounts::add_friend(char *my_name,char *fri_name)
{
	struct friendly *fri = NULL;
	account *acc1 = find_account(my_name);
	account *acc2 = find_account(fri_name);
	if(NULL == acc2)	//欲加的好友不存在
		return 1;
	if(NULL == acc1)
		return 3;
	fri = acc1->p_friend;
	while(NULL != fri->next)
	{
		if(0 == strcmp(fri->next->p_account->name,fri_name))	//好友已经被添加了
			return 2;
		fri = fri->next;
	}
	
	//将好友加入队尾
	mutex_.acquire();
	struct friendly *t_fri = new struct friendly;
	t_fri->p_account = acc2;
	t_fri->next = NULL;
	fri->next = t_fri;
	mutex_.release();
	
	return 0;
}

int Accounts::dele_friend(char *my_name,char *fri_name)
{
	struct account* acc = find_account(my_name);
	if(NULL == acc)
		return 0;
	struct friendly *fri_front = acc->p_friend;
	struct friendly *fri_back = fri_front->next;
	mutex_.acquire();
	while(NULL != fri_back)
	{
		if(!strcmp(fri_back->p_account->name,fri_name))
		{
			fri_front->next = fri_back->next;
			delete(fri_back);
			mutex_.release();
			return 0;
		}
		fri_front = fri_back;
		fri_back = fri_back->next;
	}
	mutex_.release();
	//没有找到好友
	return 1;
}

int Accounts::dele_all_friend(char *name)
{
	struct account* acc = find_account(name);
	if(NULL == acc)
		return 0;
	struct friendly *fri_front = acc->p_friend->next;
	if(NULL == fri_front)
		return 1;
	struct friendly *fri_back = fri_front->next;
	acc->p_friend->next = NULL;
	do
	{
		dele_friend(fri_front->p_account->name,name);
		mutex_.acquire();
		delete(fri_front);
		fri_front = NULL;
		if(NULL != fri_back)
		{
			fri_front = fri_back;
			fri_back = fri_back->next;
		}
		mutex_.release();
	}while(NULL != fri_front);
	
	return 1;
}

struct account* Accounts::find_account(char *name)
{
	struct account *acc = accounts_->next;
	mutex_.acquire();
	while(NULL != acc)
	{
		if(!strcmp(acc->name,name))
		{
			mutex_.release();
			return acc;
		}
		acc = acc->next;
	}
	mutex_.release();
	
	return NULL;
}

struct account* Accounts::find_friend(struct account* acc,char *name)
{
	struct friendly *fri = acc->p_friend->next;
	mutex_.acquire();
	while(NULL != fri)
	{
		if(!strcmp(fri->p_account->name,name))
		{
			mutex_.release();
			return fri->p_account;
		}
		fri = fri->next;
	}
	mutex_.release();
	
	return NULL;
}

Server_Stream* Accounts::get_stream(char *name)
{
	char *p_name = name;
	struct account *acc = find_account(p_name);
	if(NULL == acc)
		return 0;
	return acc->stream;
}

void Accounts::print_account()
{
	struct account *acc = accounts_->next;
	mutex_.acquire();
	while(NULL != acc)
	{
		ACE_OS::printf("%s\n",acc->name);
		acc = acc->next;
	}
	mutex_.release();
}
void Accounts::print_friend(char *name)
{
	struct account *acc = find_account(name);
	if(NULL == acc)
		return;
	struct friendly *fri = acc->p_friend->next;
	ACE_OS::printf("The friend of %s is: \n",name);
	mutex_.acquire();
	while(NULL != fri)
	{
		ACE_OS::printf("%s\n",fri->p_account->name);
		fri = fri->next;
	}
	mutex_.release();
}
