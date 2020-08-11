#include "WindowsServer.h"
namespace tcp
{
	void WindowsServer::Update()
	{
		for (int i = 0; i < m_Onlines->Count(); i++)
		{
			auto c = m_Onlines->Value(i);
			if (c->index == -1) continue;
			if (c->state == common::E_SSS_Free) continue;
			if (c->state >= common::E_SSS_NeedSave) continue;
			UpdateDisconnect(c);
			if (c->closeState == common::E_SSC_ShutDown) continue;
			//解析指令
			//发送数据
			this->Post_Send(c);
		}
	}

	void WindowsServer::UpdateDisconnect(S_CONNECT_BASE* c)
	{
		//1、检查安全关闭
		int temp = 0;
		if (c->closeState == common::E_SSC_ShutDown)
		{
			temp = (int)time(NULL) - c->temp_CloseTime;
			if (c->recvs.isCompleted && c->sends.isCompleted)
			{
				ReleaseSocket(c->socketfd, c, 1001);
			}
			else if (temp > 1)
			{
				ReleaseSocket(c->socketfd, c, 1002);
			}
			return;
		}
		//2、检查安全连接
		temp = (int)time(NULL) - c->temp_ConnectTime;
		if (c->state == common::E_SSS_Connect)
		{
			// 超过10秒还不是安全连接，踢出
			if (temp > 10)
			{
				ShutDownSocket(c->socketfd, 0, c, 1001);
				return;
			}
		}
		//3、检查心跳连接
		temp = (int)time(NULL) - c->temp_HeartTime;
		if (temp > common::ServerXML->maxHeartTime)
		{
			ShutDownSocket(c->socketfd, 0, c, 1002);
			return;
		}
	}

	S_CONNECT_BASE* WindowsServer::FindNoStateData()
	{
		std::lock_guard<std::mutex> guard(this->m_Mutex_NoState);

		for (int i = 0; i < m_Onlines->Count(); i++)
		{
			auto c = m_Onlines->Value(i);
			if (c->state == common::E_SSS_Free)
			{
				c->Reset();
				c->index = i;
				c->state = common::E_SSS_Connect;
				return c;
			}

		}
		return nullptr;
	}

	S_CONNECT_INDEX* WindowsServer::FindOnlinesIndex(const int socketfd)
	{
		if (socketfd < 0 || socketfd >= MAX_SOCKETFD_LEN) return nullptr;
		auto index = m_OnlinesIndexs->Value(socketfd);
		return index;
	}

	//自定义的结构体,用于TCP服务器
	typedef struct tcp_keepalive
	{
		unsigned long onoff;
		unsigned long keepalivetime;
		unsigned long keepaliveinterval;
	}TCP_KEEPALIVE, * PTCP_KEEPALIVE;

	//用于检测突然断线,只适用于windows 2000后平台
		   //即客户端也需要win2000以上平台
	int WindowsServer::SetWindowsHeart(SOCKET s)
	{
		DWORD dwError = 0L, dwBytes = 0;
		TCP_KEEPALIVE sKA_Settings = { 0 }, sReturned = { 0 };
		sKA_Settings.onoff = 1;
		sKA_Settings.keepalivetime = 5500; // Keep Alive in 5.5 sec.
		sKA_Settings.keepaliveinterval = 1000; // Resend if No-Reply
		DWORD SIO_KEEPALIVE_VALS = IOC_IN | IOC_VENDOR | 4;
		dwError = WSAIoctl(s,
			SIO_KEEPALIVE_VALS,
			&sKA_Settings, sizeof(sKA_Settings),
			&sReturned, sizeof(sReturned),
			&dwBytes,
			NULL,
			NULL);
		if (dwError == SOCKET_ERROR)
		{
			dwError = WSAGetLastError();
			LOGINFO("SetHeartCheck->WSAIoctl()发生错误,错误代码: %ld  \n", dwError);
			return -1;
		}
		return 0;
	}

	void WindowsServer::ComputeSecureNum(bool isadd)
	{
		std::lock_guard<std::mutex> guard(this->m_Mutex_SecureCount);

		if (isadd)
			m_SecurityCount++;
		else
			m_SecurityCount--;
	}

	void WindowsServer::ComputeConnectNum(bool isadd)
	{
		std::lock_guard<std::mutex> guard(this->m_Mutex_ConnectCount);

		if (isadd)
			m_ConnectCount++;
		else
			m_ConnectCount--;
	}

	bool WindowsServer::IsCloseClient(const int index, int secure)
	{
		if (index < 0 || index >= m_Onlines->Count()) return false;

		auto c = m_Onlines->Value(index);
		if (c == NULL) return false;
		if (c->state >= secure) return false;

		//ShutDownSocket
		return true;
	}

	S_CONNECT_BASE* WindowsServer::FindClient(const int socketfd, bool issecure)
	{
		if (socketfd < 0 || socketfd >= MAX_SOCKETFD_LEN) return nullptr;
		auto cindex = m_OnlinesIndexs->Value(socketfd);
		if (cindex == nullptr) return nullptr;
		if (cindex->index < 0) return nullptr;

		auto c = FindClient(cindex->index);
		if (c == nullptr)
		{
			return nullptr;
		}
		if (issecure)
		{
			if (!c->IsEqual(socketfd)) return nullptr;
		}

		return c;
	}

	S_CONNECT_BASE* WindowsServer::FindClient(const int index)
	{
		if (index < 0 || index >= m_Onlines->Count()) return nullptr;
		auto c = m_Onlines->Value(index);
		return c;
	}

	void WindowsServer::SetNotify_Connect(ISERVER_NOTIFY e)
	{
		m_Notify_Accept = e;
	}

	void WindowsServer::SetNotify_Secure(ISERVER_NOTIFY e)
	{
		m_Notify_Secure = e;
	}

	void WindowsServer::SetNotify_DisConnect(ISERVER_NOTIFY e)
	{
		m_Notify_Disconnect = e;
	}

	void WindowsServer::SetNotify_Command(ISERVER_NOTIFY e)
	{
		m_Notify_Command = e;
	}
}