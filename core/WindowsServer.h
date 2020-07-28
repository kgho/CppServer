#ifndef  ____WINDOWSERVER_H
#define ____WINDOWSERVER_H

#include "ITcp.h"

namespace tcp
{
	class WindowsServer : public IServer
	{
	public:
		WindowsServer();
		virtual ~WindowsServer();
		virtual void StartServer();
		virtual void StopServer();
		virtual bool IsCloseClient(const int index, int secure);
		virtual S_CONNECT_BASE* FindClient(const int socketfd, bool  issecure);
		virtual S_CONNECT_BASE* FindClient(const int index);
		virtual void Update();
		virtual void CreatePackage(const int index, const uint16_t cmd, void* v, const int len);
		virtual void ReadPackage(const int index, void* v, const int len);
		virtual void SetNotify_Connect(ISERVER_NOTIFY e);
		virtual void SetNotify_Secure(ISERVER_NOTIFY e);
		virtual void SetNotify_DisConnect(ISERVER_NOTIFY e);
		virtual void SetNotify_Command(ISERVER_NOTIFY e);
	};
}
#endif