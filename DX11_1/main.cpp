#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	//����ϵͳ�����
	System = new SystemClass;

	//��ʼ��ϵͳ
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}
	//�ͷ�ϵͳ��Դ
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}