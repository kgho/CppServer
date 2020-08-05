﻿#include "WindowsServer.h"

using namespace common;
using namespace recycle;

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
		int ret = InitSocket();
		if (ret < 0)
		{
			LOGINFO("InitSocket err...%d \n", ret);
			if (m_Completeport != NULL && m_Completeport != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_Completeport);
				m_Completeport = INVALID_HANDLE_VALUE;
			}
			if (m_ListenSocket != INVALID_SOCKET)
			{
				closesocket(m_ListenSocket);
				m_ListenSocket = INVALID_SOCKET;
			}
			if (ret != -2) WSACleanup();
			return;
		}
		//4、初始化投递数据
		//5、开始运行线程
		InitThread();
	}

	int WindowsServer::InitSocket()
	{
		//1、创建完成端口
		m_Completeport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (m_Completeport == NULL) return -1;
		//2、加载套接字库
		WSADATA wsData;
		int errcode = WSAStartup(MAKEWORD(2, 2), &wsData);
		if (errcode != 0) return -2;
		//3、创建套接字
		m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_ListenSocket == INVALID_SOCKET) return -3;
		//设置非阻塞模式
		unsigned long ul = 1;
		errcode = ioctlsocket(m_ListenSocket, FIONBIO, (unsigned long*)&ul);
		if (errcode == SOCKET_ERROR) return -4;
		//关闭监听的发送接收缓冲区
		int buffersize = 0;
		setsockopt(m_ListenSocket, SOL_SOCKET, SO_RCVBUF, (char*)&buffersize, sizeof(int));
		setsockopt(m_ListenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&buffersize, sizeof(int));
		//4、绑定完成端口
		HANDLE handle = CreateIoCompletionPort((HANDLE)m_ListenSocket, m_Completeport, 0, 0);
		if (handle == NULL) return -5;
		//5、绑定到IP和端口
		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(common::ServerXML->appPort);
		serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		errcode = ::bind(m_ListenSocket, (struct sockaddr*) & serAddr, sizeof(serAddr));
		if (errcode == SOCKET_ERROR) return -6;
		//6、监听socket 已经完成三次握手的队列数量
		errcode = ::listen(m_ListenSocket, SOMAXCONN);
		if (errcode == SOCKET_ERROR) return -7;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		GUID GuidGetAcceptEx = WSAID_GETACCEPTEXSOCKADDRS;
		DWORD dwBytes = 0;
		// 导出函数指针 m_AcceptEx  m_GetAcceptEx
		// 参数说明
		//1需要控制的套接口的句柄。
		//2将进行的操作的控制代码，即所需要操控的类型
		//3输入缓冲区的地址（如果想指定一个函数对套接字进行控制，可以是一个guid，指定这个控制函数的guid）。
		//4输入缓冲区的大小（这里为guid的大小，即sizeof(&guid)）。
		//5输出缓冲区的地址（这里即是函数指针的地址）。
		//6输出缓冲区的大小（函数指针的大小）。
		//7输出实际字节数的地址。
		//8WSAOVERLAPPED结构的地址（一般为NULL）。
		//9一个指向操作结束后调用的例程指针（一般为NULL）。
		if (m_AcceptEx == NULL)
		{
			errcode = WSAIoctl(m_ListenSocket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&GuidAcceptEx,
				sizeof(GuidAcceptEx),
				&m_AcceptEx,
				sizeof(m_AcceptEx),
				&dwBytes,
				NULL,
				NULL);
		}
		if (m_AcceptEx == NULL || errcode == SOCKET_ERROR) return -8;
		if (m_GetAcceptEx == NULL)
		{
			errcode = WSAIoctl(m_ListenSocket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&GuidGetAcceptEx,
				sizeof(GuidGetAcceptEx),
				&m_GetAcceptEx,
				sizeof(m_GetAcceptEx),
				&dwBytes,
				NULL,
				NULL);
		}
		if (m_GetAcceptEx == NULL || errcode == SOCKET_ERROR) return -9;
		return 0;
	}

	void WindowsServer::StopServer()
	{
	}

	void WindowsServer::CreatePackage(const int index, const uint16_t cmd, void* v, const int len)
	{
	}
	void WindowsServer::ReadPackage(const int index, void* v, const int len)
	{
	}

	void WindowsServer::InitAccept()
	{
		for (int i = 0; i < ServerXML->maxPostAccept; i++)
		{
			Post_Accept();
		}
	}

	//投递新的连接
	int WindowsServer::Post_Accept()
	{
		SOCKET socketfd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (socketfd == INVALID_SOCKET) return -1;
		ULONG ul = 1;
		int errcode = ioctlsocket(socketfd, FIONBIO, (unsigned long*)&ul);
		if (errcode == SOCKET_ERROR)
		{
			closesocket(socketfd);
			socketfd = INVALID_SOCKET;
			return -2;
		}
		auto  context = PostAcceptRecycle::Pop();
		context->m_PostSocket = socketfd;
		//参数说明
		//1、监听socket
		//2、接受连接socket
		//3、接受缓冲区 a、客户端发来第一组数据 b、server地址 c、client地址
		//4、0不会等到数据到来直接返回 非0等待数据
		//5、本地地址大小；长度必须为地址长度 + 16字节
		//6、远端地址大小；长度必须为地址长度 + 16字节
		//7、同步方式才有用 我们是异步IO没用，不用管；
		//8、本次重叠I / O所要用到的重叠结构
		unsigned long dwBytes = 0;
		errcode = m_AcceptEx(m_ListenSocket,
			context->m_PostSocket,
			context->m_buf,
			0,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&dwBytes,
			&context->m_OverLapped);
		if (errcode == false)
		{
			int error = WSAGetLastError();
			if (ERROR_IO_PENDING != error)
			{
				closesocket(socketfd);
				socketfd = INVALID_SOCKET;
				PostAcceptRecycle::Push(context);
				return -3;
			}
		}
		return 0;
	}

	int32_t WindowsServer::ReleaseSocket(SOCKET socketfd, S_CONNECT_BASE* c, int kind)
	{
		if (socketfd == SOCKET_ERROR || socketfd == INVALID_SOCKET) return  -1;
		if (c != nullptr)
		{
			if (c->state == common::E_SSS_Free) return 0;
			if (c->state >= common::E_SSS_Secure)
			{
				this->ComputeSecureNum(false);
			}
		}
		this->ComputeConnectNum(false);
		shutdown(socketfd, SD_BOTH);
		if (socketfd != INVALID_SOCKET)
		{
			closesocket(socketfd);
			socketfd = INVALID_SOCKET;
		}
		return 0;
	}

	void WindowsServer::ShutDownSocket(SOCKET s, const int32_t mode, S_CONNECT_BASE* c, int kind)
	{
		if (c != nullptr)
		{
			if (c->state == common::E_SSS_Free) return;
			if (c->closeState == common::E_SSC_ShutDown) return;
			c->temp_ShutDown = kind;
			c->temp_CloseTime = (int)time(NULL);
			c->closeState = common::E_SSC_ShutDown;
			shutdown(s, SD_BOTH);
			CancelIoEx((HANDLE)s, nullptr);
			return;
		}
		auto c2 = FindClient(s, true);
		if (c2 == nullptr)
		{
			return;
		}
		if (c2->state == common::E_SSS_Free) return;
		if (c2->closeState == common::E_SSC_ShutDown) return;
		switch (mode)
		{
		case common::E_CT_Recv:
			c2->recvs.isCompleted = true;
			break;
		case common::E_CT_Send:
			c2->sends.isCompleted = true;
			break;
		}
		c2->temp_ShutDown = kind;
		c2->temp_CloseTime = (int)time(NULL);
		c2->closeState = common::E_SSC_ShutDown;
		shutdown(s, SD_BOTH);
	}

	//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
//线程
	void WindowsServer::InitThread()
	{
		m_IsRunning = true;
		if (common::ServerXML->maxThread <= 0) common::ServerXML->maxThread = 1;
		else if (common::ServerXML->maxThread > MAX_THREAD_LEN)  common::ServerXML->maxThread = MAX_THREAD_LEN;
		for (int i = 0; i < common::ServerXML->maxThread; i++)
		{
			m_WorkThread[i].reset(new  std::thread(WindowsServer::RunThread, this, i));
		}
		for (int i = 0; i < common::ServerXML->maxThread; i++)
			m_WorkThread[i]->detach();
	}

	void pushContext(RecycleBase* context)
	{
		switch (context->m_PostType)
		{
		case common::E_CT_Accept:
			PostAcceptRecycle::Push((PostAcceptRecycle*)context);
			break;
		case common::E_CT_Recv:
			PostRecvRecycle::Push((PostRecvRecycle*)context);
			break;
		case common::E_CT_Send:
			PostSendRecycle::Push((PostSendRecycle*)context);
			break;
		}
	}

	void WindowsServer::RunThread(WindowsServer* tcp, int id)
	{
		LOGINFO("RunThread...%d \n", id);
		ULONG_PTR key = 1;
		OVERLAPPED* overlapped = nullptr;
		DWORD recvBytes = 0;
		while (tcp->m_IsRunning)
		{
			bool iscomplete = GetQueuedCompletionStatus(tcp->CompletePort(), &recvBytes, &key, &overlapped, INFINITE);
			recycle::RecycleBase* context = CONTAINING_RECORD(overlapped, recycle::RecycleBase, m_OverLapped);
			if (context == nullptr)
			{
				if (key == 1) break;
				else continue;
			}
			if (iscomplete == false)
			{
				DWORD err = GetLastError();
				if (WAIT_TIMEOUT == err) continue;
				if (overlapped != NULL)
				{
					tcp->ShutDownSocket(context->m_PostSocket, context->m_PostType, NULL, 2001);
					pushContext(context);
					continue;
				}
				tcp->ShutDownSocket(context->m_PostSocket, context->m_PostType, NULL, 2002);
				pushContext(context);
				continue;
			}
			else
			{
				if (overlapped == NULL)
				{
					LOGINFO("overlapped == NULL \n");
					break;
				}
				if ((iscomplete == true) && (overlapped != NULL))//正常操作
				{
					if (key != 0)
					{
						LOGINFO("key !=0 \n");
						continue;
					}
					if (recvBytes == 0 && (context->m_PostType == E_CT_Recv || context->m_PostType == E_CT_Send))
					{
						tcp->ShutDownSocket(context->m_PostSocket, context->m_PostType, NULL, 2003);
						pushContext(context);
						continue;
					}
					switch (context->m_PostType)
					{
					case common::E_CT_Accept://新的连接
						break;
					case common::E_CT_Recv://有新的数据
						break;
					case common::E_CT_Send://发送数据成功
						break;
					}
				}
			}
		}
		LOGINFO("exit Thread...%d \n", id);
	}
}