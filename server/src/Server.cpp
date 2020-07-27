// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "PublicEntry.h"
#include "IData.h"
#include <time.h>

int main()
{
	std::cout << "Hello World!\n";
	//common::InitPath();

	entry::Init();

	Sleep(100000);
}

