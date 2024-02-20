#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	//m_ColorShader = 0;
	//m_TextureShader = 0;
	m_LightShader = 0;
	m_Light = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];
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
	//ModelClass：：Initialize 函数现在采用将用于呈现模型的纹理的名称以及设备上下文。
	//result = m_Model->Initialize(m_Direct3D->GetDevice());
	strcpy_s(textureFilename, "../DX11_1/data/stone01.tga");
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initilize the model object", L"Error", MB_OK);
		return false;
	}

	//
	//m_ColorShader = new ColorShaderClass;
	//m_TextureShader = new TextureShaderClass;
	//此处将创建并初始化新的浅色着色器对象。

		// Create and initialize the light shader object.
	m_LightShader = new LightShaderClass;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	//result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
	//	return false;
	//}
	//result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
	//	return false;
	//}
	//此处将创建新的光源对象。 灯光的颜色设置为白色，灯光方向设置为指向正 Z 轴。

		// Create and initialize the light object.
	m_Light = new LightClass;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}


void ApplicationClass::Shutdown()
{
	// 释放着色器
	//if (m_ColorShader)
	//{
	//	m_ColorShader->Shutdown();
	//	delete m_ColorShader;
	//	m_ColorShader = 0;
	//}
	//if (m_TextureShader)
	//{
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = 0;
	//}
		// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
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
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}

#define M_PI 3.14159265358979323846
bool ApplicationClass::Frame()
{
	static double angle = 0.0;
	angle += 10.0;
	double cosval = _CMATH_::cos(angle * (M_PI / 180));
	m_Light->SetDirection((float)cosval, 0.0f, 1.0f);

	static float rotation = 0.0f;
	//每帧执行渲染
	bool result;

	// Update the rotation variable each frame.
	rotation -= 0.0174532925f * 0.5f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	result = Render(rotation);
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

bool ApplicationClass::Render(float rotation)
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

	//在这里，我们通过旋转值旋转世界矩阵，以便当我们使用这个更新的世界矩阵渲染三角形时，它将按旋转量旋转三角形。

		// Rotate the world matrix by the rotation value so that the triangle will spin.
	worldMatrix = XMMatrixRotationY(rotation);

	//将模型顶点和索引缓冲区放在图形管道上，为绘制做好准备。
	m_Model->Render(m_Direct3D->GetDeviceContext());



	//使用着色器渲染模型
	//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	//if (!result)
	//{
	//	return false;
	//}
	//现在调用纹理着色器而不是颜色着色器来渲染模型。 请注意，它还从模型中获取纹理资源指针，以便纹理着色器可以访问模型对象中的纹理。
	//result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());

	//此处调用浅色着色器来渲染三角形。 新的光源对象用于将漫反射光源颜色和光源方向发送到 Render 函数中，以便着色器能够访问这些值。

		// Render the model using the light shader.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	if (!result)
	{
		return false;
	}

	//将渲染的场景呈现到屏幕上。
	m_Direct3D->EndScene();
	return true;
}
