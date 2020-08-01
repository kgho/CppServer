#include "WindowsServer.h"
namespace tcp
{
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