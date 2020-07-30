#include "WindowsServer.h"
namespace tcp
{
	IServer* tcp::NewIServer()
	{
		return new WindowsServer();
	}
	WindowsServer::WindowsServer()
	{
		m_AcceptEx = NULL;
		m_GetAcceptEx = NULL;
		m_ListenSocket = INVALID_SOCKET;
		m_Completeport = NULL;
		m_ConnectCount = 0;
		m_SecurityCount = 0;
		m_IsRunning = false;
		m_Onlines = NULL;
		m_OnlinesIndexs = NULL;
		m_Notify_Accept = NULL;
		m_Notify_Secure = NULL;
		m_Notify_Disconnect = NULL;
		m_Notify_Command = NULL;
	}
	WindowsServer::~WindowsServer()
	{
	}
	void WindowsServer::StartServer()
	{
		//1、创建连接用户
		m_Onlines = new  common::HashContainer<S_CONNECT_BASE>(common::ServerXML->appMaxConnect);
		for (int i = 0; i < m_Onlines->Count(); i++)
		{
			auto c = m_Onlines->Value(i);
			c->Init();
		}
		//2、创建连接用户索引
		m_OnlinesIndexs = new  common::HashContainer<S_CONNECT_INDEX>(MAX_SOCKETFD_LEN);
		for (int i = 0; i < m_OnlinesIndexs->Count(); i++)
		{
			auto index = m_OnlinesIndexs->Value(i);
			index->Reset();
		}
		//3、初始化socket
		//4、初始化投递数据
		//5、开始运行线程
	}
	void WindowsServer::StopServer()
	{
	}
	bool WindowsServer::IsCloseClient(const int index, int secure)
	{
		return false;
	}
	S_CONNECT_BASE* WindowsServer::FindClient(const int socketfd, bool  issecure)
	{
		return nullptr;
	}
	S_CONNECT_BASE* WindowsServer::FindClient(const int index)
	{
		return nullptr;
	}
	void WindowsServer::Update()
	{
	}
	void WindowsServer::CreatePackage(const int index, const uint16_t cmd, void* v, const int len)
	{
	}
	void WindowsServer::ReadPackage(const int index, void* v, const int len)
	{
	}
	void WindowsServer::SetNotify_Connect(ISERVER_NOTIFY e)
	{
	}
	void WindowsServer::SetNotify_Secure(ISERVER_NOTIFY e)
	{
	}
	void WindowsServer::SetNotify_DisConnect(ISERVER_NOTIFY e)
	{
	}
	void WindowsServer::SetNotify_Command(ISERVER_NOTIFY e)
	{
	}
}