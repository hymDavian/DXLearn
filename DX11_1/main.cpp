#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	//创建系统类对象
	System = new SystemClass;

	//初始化系统
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}
	//释放系统资源
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}