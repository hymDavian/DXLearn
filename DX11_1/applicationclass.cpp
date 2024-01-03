#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	m_Direct3D = new D3DClass;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not iniltialize Direct3D", L"Error", MB_OK);
		return false;
	}
	//初始化相机，模型，着色器类
	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);//初始化到后5个单位的位置

	m_Model = new ModelClass;
	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initilize the model object", L"Error", MB_OK);
		return false;
	}

	m_ColorShader = new ColorShaderClass;
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}


void ApplicationClass::Shutdown()
{
	// 释放着色器
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// 释放模型类
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 释放相机类
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	//释放dx3d对象
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}


bool ApplicationClass::Frame()
{
	//每帧执行渲染
	bool result;
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

/*从清除场景开始，清除为黑色。 之后，调用相机对象的 Render 函数，以根据 Initialize 函数中设置的相机位置创建视图矩阵。 
创建视图矩阵后，从 camera 类中获取它的副本。 还从 D3DClass 对象获取世界和投影矩阵的副本。 
然后，调用 ModelClass：：Render 函数将绿色三角形模型几何图形放在图形管道上。 
现在顶点准备好了，调用颜色着色器来绘制顶点，使用模型信息和三个矩阵来定位每个顶点。 
绿色三角形现在被绘制到后面的缓冲区。这样，场景就完成了，调用 EndScene 将其显示在屏幕上*/

bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	
	//清除缓冲区以开始场景。背景黑色
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//根据相机位置生成视图矩阵
	m_Camera->Render();

	//从相机和对d对象获取 世界，视图，投影矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//将模型顶点和索引缓冲区放在图形管道上，为绘制做好准备。
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//使用着色器渲染模型
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//将渲染的场景呈现到屏幕上。
	m_Direct3D->EndScene();
	return true;
}
