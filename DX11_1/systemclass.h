#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

//��������ȥ��һЩ�����õ�API
#define WIN32_LEAN_AND_MEAN
//����windows��api
#include <windows.h>
//��Ӧ�õ�ʵ���������Ӧ������������
#include "inputclass.h"
#include "applicationclass.h"
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();
	//��ʼ��
	bool Initialize();
	//����
	void Shutdown();
	//����
	void Run();
	//�����յ�������
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	//֡����
	bool Frame();
	//��ʼ������
	void InitializeWindows(int&, int&);
	//�رմ���
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//ȫ�ַ��ʵ�Ӧ��ָ��
static SystemClass* ApplicationHandle = 0;


#endif