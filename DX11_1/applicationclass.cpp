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
	//��ʼ�������ģ�ͣ���ɫ����
	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);//��ʼ������5����λ��λ��

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
	// �ͷ���ɫ��
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
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
	//ÿִ֡����Ⱦ
	bool result;
	result = Render();
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

bool ApplicationClass::Render()
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

	//��ģ�Ͷ������������������ͼ�ιܵ��ϣ�Ϊ��������׼����
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//ʹ����ɫ����Ⱦģ��
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//����Ⱦ�ĳ������ֵ���Ļ�ϡ�
	m_Direct3D->EndScene();
	return true;
}
