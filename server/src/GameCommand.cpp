#include "GameCommand.h"
namespace app
{
	tcp::IServer* __IServer = nullptr;
	void Event_Server_Accept(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const  int32_t code)
	{
	}
	void Event_Server_Secure(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const  int32_t code)
	{
	}
	void Event_Server_Disconnect(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const int32_t code)
	{
	}
	void Event_Server_Command(tcp::IServer* tcp, tcp::S_CONNECT_BASE* c, const  int32_t cmd)
	{
	}
}