#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

//������ɫ���ĳ�ʼ�������� ���� HLSL ��ɫ���ļ�������
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];//������ɫ��Դ���ļ���ַ
	wchar_t psFilename[128];//������ɫ��Դ���ļ���ַ
	int error;

	//���ö�����ɫ���ļ�
	//wcscpy_s �������ַ����İ�ȫ���� ���� error_t ����ֵ
	error = wcscpy_s(vsFilename, 128, L"../DX11_1/color.vs");
	if (error != 0)
	{
		return false;
	}
	//����������ɫ���ļ�
	error = wcscpy_s(psFilename, 128, L"../DX11_1/color.ps");
	if (error != 0)
	{
		return false;
	}

	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}


	return true;
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader();
	return;
}

//Render ������ʹ�� SetShaderParameters ��������ɫ�������ò����� ���ò������������ RenderShader ��ʹ�� HLSL ��ɫ��������ɫ�����Ρ�

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	bool result;
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	RenderShader(deviceContext, indexCount);
	return true;
}

/* InitializeShader ������ɫ���ļ���ʹ������� DirectX �� GPU�� 
���ֵ������Լ����㻺���������� GPU ��ͼ�ιܵ��ϵ���ۡ� 
������Ҫƥ�� modelclass.h �ļ��е� VertexType �Լ� color.vs �ļ��ж���� VertexType��*/
//��ʼ��shader
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	//ID3D10Blob �������ⳤ�ȵ�����

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];//����װ�����׶εĵ���Ԫ�ص�˵��
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;//���󻺳�������

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	/*���������ɫ���������Ϊ�������� 
	�ṩ��ɫ���ļ������ơ���ɫ�������ơ���ɫ���汾��DirectX 11 �е� 5.0���Լ����ڱ�����ɫ���Ļ������� 
	���������ɫ��ʧ�ܣ������� errorMessage �ַ����з���һ��������Ϣ�������ַ������͵���һ��������д������ 
	�������Ȼʧ�ܲ���û�� errorMessage �ַ��������ʾ���Ҳ�����ɫ���ļ�������������£�����һ���Ի�����˵����һ�㡣*/

	//���붥����ɫ������ ���������ǲ�������
	//Դ���ļ���ַ��
	// ������ɫ���� �Ŀ�ѡD3D_SHADER_MACRO �ṹ���飬
	// ָ�� ID3DInclude �ӿڵĿ�ѡָ�룬������ʹ�ø�ָ������������ļ���
	// ��ɫ��ִ�п�ʼ������ɫ����ڵ㺯��������
	// Ҫ�������ɫ��Ŀ�꣬��..����ʾ������ɫ��
	// ָ����������α��� HLSL ����,
	// ���ɵ�ֵָ����������α���Ч��������Ǳ�����ɫ��������ԣ�������Ϊ0
	// ����ָ������ڷ����ѱ�������ָ��
	// ����ָ������ڷ��ʱ�����������Ϣ��ָ��
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//�����ɫ��δ�ܱ��룬��Ӧ���Ѿ��������Ϣд����һЩ���ݡ�
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else //���������Ϣ��û���κ����ݣ���ô�������Ҳ�����ɫ���ļ�����
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//����������ɫ��
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	/*һ��������ɫ����������ɫ������ɹ����뵽�������У�
	ʹ����Щ��������������ɫ�������� �����ڿ�ʼ����ʹ����Щָ���붥���������ɫ�����н�����*/

	//�ӻ���������������ɫ������
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//������ɫ��
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*��һ���Ǵ���������ɫ������Ķ������ݵĲ��֡� ���ڴ���ɫ��ʹ��λ�ú���ɫʸ���������Ҫ�ڲ����д������ߣ�ָ�����ߵĴ�С�� 
	���������ǲ���������Ҫ��д�����ݣ���������ɫ��ȷ�������д�Ԫ�ص��÷���
	������������ͬ��Ԫ�أ�ʹ�� POSITION ��Ϊ��һ��Ԫ�أ��� COLOR ���ڵڶ���Ԫ�ء�
	���ֵ���һ����Ҫ�����Ǹ�ʽ�� ����λ��������ʹ�� DXGI_FORMAT_R32G32B32_FLOAT��������ɫ��ʹ�� DXGI_FORMAT_R32G32B32A32_FLOAT�� 
	��Ҫע������һ������ AlignedByteOffset����ָʾ�����ڻ������еļ����ʽ�� 
	���ڴ˲��֣�ǰ 12 ���ֽ���λ�ã��������� 16 ���ֽڽ�����ɫ��AlignedByteOffset ��ʾÿ��Ԫ�صĿ�ʼλ�á�
	����ʹ�� D3D11_APPEND_ALIGNED_ELEMENT �����ǽ��Լ���ֵ���� AlignedByteOffset �У������������*/

	//�����������벼������
	//��������Ҫƥ��ModelClass����ɫ���е�VertexType�ṹ
	polygonLayout[0].SemanticName = "POSITIONT";//��ɫ������ǩ�������Ԫ�ع����� HLSL ����
	polygonLayout[0].SemanticIndex = 0;//Ԫ�ص����������� ��������ʹ�������������޸����塣 �����ж��������ͬ�����Ԫ��ʱ������Ҫ���������� ���磬4x4 �������ĸ������ÿ�����������������
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;//Ԫ�����ݵ���������,����һ�������� 96 λ�����ʽ
	polygonLayout[0].InputSlot = 0;//��ʶ����װ���� (�����) ������ֵ�� ��Чֵ���� 0 �� 15 ֮��
	polygonLayout[0].AlignedByteOffset = 0;//��ǰԪ��ֵƫ��
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//��ʶ��������۵����������࣬��������������ÿ���������ݡ�
	polygonLayout[0].InstanceDataStepRate = 0;//�ڻ���������ǰ�ƽ�һ��Ԫ��֮ǰ��ʹ����ͬÿ��ʵ�����ݻ��Ƶ�ʵ������ ���ڰ���ÿ���������ݵ�Ԫ�أ���ֵ����Ϊ 0

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	return false;
}

void ColorShaderClass::ShutdownShader()
{
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*)
{
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX)
{
	return false;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext*, int)
{
}
