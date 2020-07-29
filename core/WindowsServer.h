#ifndef  ____WINDOWSERVER_H
#define ____WINDOWSERVER_H

#include "ITcp.h"
#include <mutex>
#include <thread>
#include  <MSWSock.h>

#define ACCEPT_BUF_LENGTH  ((sizeof(struct sockaddr_in) + 16))*2

//对象回收池
namespace recycle
{
	class RecycleBase
	{
	public:
		RecycleBase();
		~RecycleBase();
	public:
		WSAOVERLAPPED m_OverLapped;//重叠结构 可以当成是一个ID
		SOCKET        m_PostSocket;
		int           m_PostType;
	};
	//1、投递新的连接
	class PostAcceptRecycle :public RecycleBase
	{
	public:
		PostAcceptRecycle(int type);
		~PostAcceptRecycle();
	public:
		unsigned char m_buf[ACCEPT_BUF_LENGTH];
		void Clear();
		static PostAcceptRecycle* Pop();
		static void Push(PostAcceptRecycle* acc);
	};
	//2、投递新的接收数据
	class PostRecvRecycle :public RecycleBase
	{
	public:
		PostRecvRecycle(int type);
		~PostRecvRecycle();
	private:
		char* m_Buffs;
	public:
		WSABUF m_wsaBuf;//每次的操作缓冲区
		void Clear();
		static PostRecvRecycle* Pop();
		static void Push(PostRecvRecycle* acc);
	};
	//3、投递新的发送数据
	class PostSendRecycle :public RecycleBase
	{
	public:
		PostSendRecycle(int type);
		~PostSendRecycle();
	private:
		char* m_Buffs;//WSABUF里具体字符的缓冲区
	public:
		WSABUF m_wsaBuf;//每次的操作缓冲区
		void Clear();
		int SetPostSend(SOCKET s, char* data, const int sendByte);
		static PostSendRecycle* Pop();
		static void Push(PostSendRecycle* acc);
	};
}

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