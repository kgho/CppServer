﻿#include "IData.h"
#include <string>

#ifdef ____Win32
#else
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#endif


namespace common
{
	char FileExePath[MAX_FILE_LEN];
	ConfigXML* ServerXML = NULL;
	ConfigXML* ClientXML = NULL;
	std::vector<ServerListXML*> ServerXMLS;
	void(*MD5_FunPoint)(char* output, unsigned char* input, int len) = NULL;

	void InitPath()
	{
		memset(FileExePath, 0, MAX_FILE_LEN);
#ifdef ____Win32
		GetModuleFileNameA(NULL, (LPSTR)FileExePath, MAX_FILE_LEN);

		std::string str(FileExePath);
		size_t pos = str.find_last_of("\\");
		str = str.substr(0, pos + 1);
		memcpy(FileExePath, str.c_str(), MAX_FILE_LEN);

		LOGINFO("path:%s \n", FileExePath);
#else
		int ret = readlink("/proc/self/exe", FileExePath, MAX_FILE_LEN);
		std::string str(FileExePath);
		size_t pos = str.find_last_of("/");
		str = str.substr(0, pos + 1);
		memcpy(FileExePath, str.c_str(), MAX_FILE_LEN);
		LOGINFO("path:%s \n", FileExePath);
#endif


	}

	uint8_t GetServerType(int32_t sid)
	{
		if (sid >= 10000 && sid < 20000)      return common::E_APP_DB;
		else if (sid >= 20000 && sid < 30000) return common::E_APP_Center;
		else if (sid >= 30000 && sid < 40000) return common::E_APP_Game;
		else if (sid >= 40000 && sid < 50000) return common::E_APP_Gate;
		else if (sid >= 50000 && sid < 60000) return common::E_APP_Login;
		return common::E_APP_Player;
	}
}