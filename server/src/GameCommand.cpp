﻿#include "GameCommand.h"
namespace app
{
	tcp::IServer* __IServer = nullptr;

	void Event_Server_Accept(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const  int32_t code)
	{
		if (c == nullptr || tcp == nullptr) return;
		//LOGINFO("connect...%d [%s-%d] %d\n", (int)c->socketfd, c->ip,  c->port, code);
	}

	void Event_Server_Secure(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const  int32_t code)
	{
		if (c == nullptr || tcp == nullptr)   return;
		int aa = tcp->ConnectCount();
		int bb = tcp->SecureCount();
		common::SetConsoleColor(10);
		LOGINFO("secure...%d [%s:%d][connect:%d-%d]\n", (int)c->socketfd, c->ip, c->port, aa, bb);
		common::SetConsoleColor(7);
	}

	void Event_Server_Disconnect(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const int32_t code)
	{
		if (c == nullptr || tcp == nullptr)   return;
		int aa = tcp->ConnectCount();
		int bb = tcp->SecureCount();
		common::SetConsoleColor(8);
		LOGINFO("disconnect...%d [%s:%d][connect:%d-%d]  %d-%d\n", (int)c->socketfd, c->ip, c->port, aa, bb, c->temp_ShutDown, code);
		common::SetConsoleColor(7);
		if (c->state == common::E_SSS_Connect || c->state == common::E_SSS_Secure)
		{
			c->Reset();
		}
		else if (c->state == common::E_SSS_Login)
		{
			c->state = common::E_SSS_NeedSave;
			LOGINFO("user E_SSS_NeedSave...%d-%d \n", c->index, (int)c->socketfd);
		}
	}

	//消息指令集
	void Event_Server_Command(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const  int32_t cmd)
	{
		if (tcp->IsCloseClient(c->index, common::E_SSS_Secure))
		{
			LOGINFO("gamecommand is error... %d \n", c->index);
			return;
		}
		switch (cmd)
		{
		case 1000:
			break;
		}
	}
}