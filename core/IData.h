﻿#ifndef  ____IDATA_H
#define  ____IDATA_H

#include <vector>
#include <cstdint>

#ifdef ____Win32
#include <winsock2.h>
#else
#endif

#define LOGINFO             printf
#define MAX_SOCKETFD_LEN    1000000
#define MAX_FILE_LEN        250
#define MAX_MD5_LEN         35
#define MAX_THREAD_LEN      5
#define MAX_IP_LEN          20 

namespace common
{
	enum E_CONTEXT_TYPE
	{
		E_CT_Accept = 0,
		E_CT_Recv = 1,
		E_CT_Send = 2
	};

	//服务器
	enum E_SERVER_SOCKET_STATE
	{
		E_SSS_Free = 0,
		E_SSS_Connect = 1,
		E_SSS_Secure = 2,
		E_SSS_Login = 3,
		E_SSS_NeedSave = 4,
		E_SSS_Saving = 5
	};
	//客户端SOCKET枚举状态
	enum E_CLIENT_SOCKET_STATE
	{
		E_CSS_Free = 0,
		E_CSS_TryConnect = 1,
		E_CSS_Connect = 2,
		E_CSS_Secure = 3,
		E_CSS_Login = 4
	};
	//关闭状态
	enum E_SERVER_SOCKET_CLOSESTATE
	{
		E_SSC_Free = 0,
		E_SSC_Accept = 1,   //连接出错关闭
		E_SSC_ShutDown = 2, //关闭连接
		E_SSC_Close = 3     //正式关闭
	};
	//0 玩家 1-DB 2-中心服务器 3-游戏服务器 4-网关服务器 5-登录服务器
	enum E_APP_TYPE
	{
		E_APP_Player = 0,
		E_APP_DB = 1,
		E_APP_Center = 2,
		E_APP_Game = 3,
		E_APP_Gate = 4,
		E_APP_Login = 5
	};

	struct ConfigXML
	{
		uint16_t  appPort;   //服务器端口号
		int32_t   appID;     //服务器ID
		int32_t   appMaxPlayer;//最大玩家数量
		int32_t   appMaxConnect;//最大客户端连接数量
		uint8_t   appXorCode; //异或码 主要用于数据加密
		int32_t   appVersion; //当前应用程序版本号
		int32_t   recvBytesOne;//当前一次接收数据字节长度
		int32_t   recvBytesMax;//最大接收缓冲区
		int32_t   sendBytesOne;//当前一次发送数据字节长度
		int32_t   sendBytesMax;//最大发送缓冲区
		int32_t   maxHeartTime;//心跳时间
		int32_t   autoConnectTime;//自动重连时间
		int32_t   maxPostAccept; //最大投递连接数量
		int32_t   maxPostRecv;   //最大收到消息数量
		int32_t   maxPostSend;   //最大发送信息数量
		int32_t   maxThread;     //设置当前最大线程
		char  SecureCode[20];
		char  Head[3];
	};

	struct ServerListXML
	{
		int32_t    ID;
		uint16_t   Port;
		char       IP[16];
	};

	extern char FileExePath[MAX_FILE_LEN];
	extern ConfigXML* ServerXML;
	extern ConfigXML* ClientXML;
	extern std::vector<ServerListXML*> ServerXMLS;

	extern uint8_t GetServerType(int32_t sid);
	extern void(*MD5_FunPoint)(char* output, unsigned char* input, int len);
	extern void InitPath();

}

#endif // ____IDATA_H
