#ifndef  ____PUBLICENTRY_H
#define  ____PUBLICENTRY_H

#include "stdint.h"

#define CMD_REIGSTER      100
#define CMD_UPDATELOGIN   200
#define CMD_CREATEROLE    300

#define CMD_LOGIN      1000
#define CMD_MOVE       2000
#define CMD_PLAYERDATA 3000
#define CMD_LEAVE      4000

#pragma pack(push,packing)
#pragma pack(1)
struct  S_TEST_BASE
{
	uint32_t   id;
	uint32_t   curtime;
	uint8_t    aa[1024];
};
#pragma pack(pop, packing)

namespace entry
{
	extern bool Init();
}

#endif