#include "GameManager.h"
#include "GameCommand.h"
#include "PublicEntry.h"
namespace app
{
	void Init()
	{
		__IServer = tcp::NewIServer();
		__IServer->SetNotify_Connect(Event_Server_Accept);
		__IServer->SetNotify_Secure(Event_Server_Secure);
		__IServer->SetNotify_DisConnect(Event_Server_Disconnect);
		__IServer->SetNotify_Command(Event_Server_Command);
		__IServer->StartServer();
	}
	void Update()
	{
		__IServer->Update();
	}
	int StartApp()
	{
		if (!entry::Init()) return -1;
		Init();
		while (true)
		{
			Update();
#ifdef  ____Win32
			Sleep(2);
#else
			usleep(2 * 1000);
#endif
		}
		return 0;
	}
}