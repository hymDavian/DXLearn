#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

//这个宏可以去除一些不常用的API
#define WIN32_LEAN_AND_MEAN
//包含windows的api
#include <windows.h>
//此应用的实际输入类和应用生命周期类
#include "inputclass.h"
#include "applicationclass.h"
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();
	//初始化
	bool Initialize();
	//结束
	void Shutdown();
	//运行
	void Run();
	//监听收到的输入
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	//帧更新
	bool Frame();
	//初始化窗口
	void InitializeWindows(int&, int&);
	//关闭窗口
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//全局访问的应用指针
static SystemClass* ApplicationHandle = 0;


#endif