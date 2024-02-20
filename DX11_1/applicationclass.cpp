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
	//��ʼ�������ģ�ͣ���ɫ����
	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);//��ʼ������5����λ��λ��

	m_Model = new ModelClass;
	//ModelClass����Initialize �������ڲ��ý����ڳ���ģ�͵�����������Լ��豸�����ġ�
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
	//�˴�����������ʼ���µ�ǳɫ��ɫ������

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
	//�˴��������µĹ�Դ���� �ƹ����ɫ����Ϊ��ɫ���ƹⷽ������Ϊָ���� Z �ᡣ

		// Create and initialize the light object.
	m_Light = new LightClass;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}


void ApplicationClass::Shutdown()
{
	// �ͷ���ɫ��
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

	// �ͷ�ģ����
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// �ͷ������
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	//�ͷ�dx3d����
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
	//ÿִ֡����Ⱦ
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

/*�����������ʼ�����Ϊ��ɫ�� ֮�󣬵����������� Render �������Ը��� Initialize ���������õ����λ�ô�����ͼ����
������ͼ����󣬴� camera ���л�ȡ���ĸ����� ���� D3DClass �����ȡ�����ͶӰ����ĸ�����
Ȼ�󣬵��� ModelClass����Render ��������ɫ������ģ�ͼ���ͼ�η���ͼ�ιܵ��ϡ�
���ڶ���׼�����ˣ�������ɫ��ɫ�������ƶ��㣬ʹ��ģ����Ϣ��������������λÿ�����㡣
��ɫ���������ڱ����Ƶ�����Ļ�����������������������ˣ����� EndScene ������ʾ����Ļ��*/

bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//����������Կ�ʼ������������ɫ
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//�������λ��������ͼ����
	m_Camera->Render();

	//������Ͷ�d�����ȡ ���磬��ͼ��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//���������ͨ����תֵ��ת��������Ա㵱����ʹ��������µ����������Ⱦ������ʱ����������ת����ת�����Ρ�

		// Rotate the world matrix by the rotation value so that the triangle will spin.
	worldMatrix = XMMatrixRotationY(rotation);

	//��ģ�Ͷ������������������ͼ�ιܵ��ϣ�Ϊ��������׼����
	m_Model->Render(m_Direct3D->GetDeviceContext());



	//ʹ����ɫ����Ⱦģ��
	//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	//if (!result)
	//{
	//	return false;
	//}
	//���ڵ���������ɫ����������ɫ��ɫ������Ⱦģ�͡� ��ע�⣬������ģ���л�ȡ������Դָ�룬�Ա�������ɫ�����Է���ģ�Ͷ����е�����
	//result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());

	//�˴�����ǳɫ��ɫ������Ⱦ�����Ρ� �µĹ�Դ�������ڽ��������Դ��ɫ�͹�Դ�����͵� Render �����У��Ա���ɫ���ܹ�������Щֵ��

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

	//����Ⱦ�ĳ������ֵ���Ļ�ϡ�
	m_Direct3D->EndScene();
	return true;
}
