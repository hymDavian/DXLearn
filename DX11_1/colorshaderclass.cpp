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
	D3D11_BUFFER_DESC matrixBufferDesc;//��̬����������������

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

	//��ȡ����Ҫ�ص�����
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//�����������벼��
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//�ͷŶ����������ɫ������������Ϊһ���������֣��Ͳ�����Ҫ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	/*ʹ����ɫ����Ҫ���õ����һ�����ǳ�����������
	Ŀǰֻ��һ�����������������ֻ��Ҫ�ڴ˴�����һ�����������Ա��������ɫ�������� 
	������ʹ����Ҫ����Ϊ��̬����Ϊ����ÿһ֡��������
	�󶨱�־ָʾ�˻���������һ�������������� 
	CPU ���ʱ�־��Ҫ��ʹ�����ƥ�䣬��˽�������Ϊ D3D11_CPU_ACCESS_WRITE�� 
	��д�����󣬿��Դ��������������ӿڣ�Ȼ��ʹ�ú��� SetShaderParameters ������ɫ���е��ڲ�������*/

	//���ö�����ɫ���ж�̬��������������������
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;//Ԥ����ζ�ȡ��д�뻺����,���� ����ÿ֡�� CPU ���ٸ���һ�ε���Դ����̬��Դ��һ�������ѡ��
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//����������Ϊ�����������󶨵���ɫ���׶�
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU�������ͣ�������Դ�ǿ�ӳ��ģ��Ա� CPU ���Ը���������
	matrixBufferDesc.MiscFlags = 0;//���0��ʹ��
	matrixBufferDesc.StructureByteStride = 0;//��������ʾ�ṹ��������ʱ���������ṹ��ÿ��Ԫ�صĴ�С

	//��������������ָ�룬�Ա���ԴӴ����з��ʶ�����ɫ��������������
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

//�ͷ���ɫ����Դ�ļ����ӿڶ���
void ColorShaderClass::ShutdownShader()
{
	//�ͷų������󻺳���
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	//�ͷŲ�������
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}
	//�ͷŶ�����ɫ��
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	//�ͷ�������ɫ��
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	return;
}

//���ﴦ�� �ڱ��붥����ɫ����������ɫ��ʱ���ɵĴ�����Ϣ

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	//��ȡһ��������Ϣ�ı���������ָ��
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//��ȡ��Ϣ����
	bufferSize = errorMessage->GetBufferSize();

	//���ļ�����д�������Ϣ������
	fout.open("shader-error.txt");
	//д��
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	//�ر��ļ�
	fout.close();

	//�ͷŴ�����Ϣ
	errorMessage->Release();
	errorMessage = 0;

	//����Ļ�ϵ���һ����Ϣ��֪ͨ�û�����ı��ļ��еı������
	MessageBox(hwnd, L"Error compiling shader.Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

/*SetShaderVariables �����Ĵ�����Ϊ�˸����ɵ�����ɫ��������ȫ�ֱ����� 
�˺�����ʹ�õľ������� ApplicationClass �д����ġ� 
֮�󣬵��ô˺������� Render ���������ڼ佫���Ǵ����﷢�͵�������ɫ����*/

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//��Ҫȷ���ڽ������͵���ɫ��֮ǰ�������ת�ã�����DX11��Ҫ��
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//����m_matrixBuffer�������������¾���Ȼ���������
	//Map��������ȡָ�� ����Դ�а��������ݵ�ָ�룬���ܾ� GPU ���ʸ�����Դ
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	//��ȡָ���������������ݵ�ָ��
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//�������󵽳���������
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	//����
	deviceContext->Unmap(m_matrixBuffer, 0);

	//���ڣ��� HLSL ������ɫ�������ø��µľ��󻺳�����
	//���ó����������ڶ�����ɫ���е�λ��
	bufferNumber = 0;
	//���ʹ�ø��µ�ֵ�ڶ�����ɫ�������ó���������
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);


	return true;
}

/*�˺����ĵ�һ���������������н����벼������Ϊ�״̬�� 
���� GPU ֪�����㻺���������ݵĸ�ʽ�� 
�ڶ��������ö�����ɫ����������ɫ������������Ⱦ�˶��㻺������ 
������ɫ����ͨ��ʹ�� D3D �豸�����ĵ��� DrawIndexed DirectX 11 ���������������Ρ� 

���ô˺���������������ɫ�����Ρ�*/

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//���ö������벼��
	deviceContext->IASetInputLayout(m_layout);

	//���ý�������Ⱦ�������εĶ����������ɫ��
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//��Ⱦ������
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
