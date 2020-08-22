#pragma once
#ifndef  ____GAMEDATA_H
#define  ____GAMEDATA_H
#include <map>
#include <string>
#include <cstring>
#include "IData.h"
#define USER_MAX_MEMBER 20
namespace app
{
	enum E_MEMBER_STATE
	{
		M_FREE = 0x00,       //ʹ��-����
		M_REQUESTING,   //��������������
		M_LOGIN,             //ʹ��-��¼��
		M_SAVING,       //������
		M_LIMIT              //����        
	};
#pragma pack(push,packing)
#pragma pack(1)
	//�˺���Ϣ
	struct S_USER_MEMBER_BASE
	{
		inline bool IsT() const { return ID > 0; }
		inline bool IsT_ID(const __int64 id) { return (ID == id); }
		inline bool IsT_Name(const char* fname) { return (stricmp(name, fname) == 0); };
		E_MEMBER_STATE                    state;      //�˺�״̬
		int                                   ID;         //�˺�Ψһ���
		char                              name[USER_MAX_MEMBER];   //�˺�
		char                    password[USER_MAX_MEMBER];//����
		int                                      timeCreate;//�˺Ŵ���ʱ��
		int                                      timeLastLogin;//�ϴε�¼ʱ��
	};
	struct S_VECTOR
	{
		float x;
		float y;
		float z;
	};
	struct  S_PLAYER_BASE
	{
		int32_t  memid;
		int32_t  socketfd;
		int32_t  state;
		int32_t  curhp;
		int32_t  maxhp;
		float  speed;
		S_VECTOR   pos;
		S_VECTOR   rot;
		inline void init()
		{
			memset(this, 0, sizeof(S_PLAYER_BASE));
		}
	};
	struct  S_PLAYER_MOVE
	{
		int32_t memid;
		float   speed;
		S_VECTOR pos;
		S_VECTOR rot;
	};
	struct  S_PLAYER_MEMID
	{
		int32_t memid;
	};
	struct S_REGISTER_BASE
	{
		char name[USER_MAX_MEMBER];
		char password[USER_MAX_MEMBER];
		int         socketfd;
		int               ID;
	};
	struct S_LOGIN_BASE
	{
		char name[USER_MAX_MEMBER];
		char password[USER_MAX_MEMBER];
		int         socketfd;
		int64_t              ID;
	};
	struct  S_ERROR_BASE
	{
		int32_t value;
	};
	struct  S_LOGIN_RETURN
	{
		int kind;
		S_PLAYER_BASE  player;
	};
#pragma pack(pop, packing)
	extern std::map<std::string, S_USER_MEMBER_BASE*>   __AccountsName;
	extern std::map<int, S_USER_MEMBER_BASE*>           __AccountsID;
	extern S_USER_MEMBER_BASE* FindMember(std::string name);
	extern S_USER_MEMBER_BASE* FindMember(int memid);
}
#endif