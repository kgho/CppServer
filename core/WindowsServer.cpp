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