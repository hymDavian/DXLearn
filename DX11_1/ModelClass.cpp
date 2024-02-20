#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool result;
	//��ʼ�����������������
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}


	return true;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();

	//���û������ر�
	ShutdownBuffers();
	return;
}

/*Render �Ǵ� ApplicationClass����Render �������õġ� 
�˺������� RenderBuffers ���������������������ͼ�ιܵ��ϣ��Ա���ɫ��ɫ���ܹ��������ǡ�*/

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

//����ģ���е�����������ɫ��ɫ������Ҫ����Ϣ�����ƴ�ģ��
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

/*���������������������,ͨ���������ģ�Ͳ��Ӹ������ļ�����������*/

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	/*���ȴ���������ʱ���������涥����������ݣ�
	�Ժ�ʹ����Щ������������ջ�������*/

	//���ö������������ 
	//��������Ϊ��
	m_vertexCount = 3;

	//�����������������
	m_indexCount = 3;

	//������������
	vertices = new VertexType[m_vertexCount];
	if (!vertices) { return false; }

	//������������
	indices = new unsigned long[m_indexCount];
	if (!indices) { return false; }

	/*�����������ε��������Լ�ÿ�����������䶥����������顣 
	��ע�⣬���ջ��Ƶ��˳ʱ��˳�򴴽��㡣 �����ʱ����������������Ϊ�����γ����෴�ķ��򣬲������ڱ����޳���û�л�������
	�����㷢�͵� GPU ��˳��ǳ���Ҫ�� ��ɫҲ�ڴ˴����ã���Ϊ���Ƕ���������һ���֡�*/

	/*�����������ھ������������������������ɫ������ 
	��������ʼ���� U ��ǰ��V �ں� 
	���磬��һ����������λ�������ε����½ǣ���Ӧ�� U 0.0�� V 1.0�� 
	ʹ�ô�ҳ�涥����ͼ����ȷ������������Ҫ��ʲô�� 
	��ע�⣬�����Ը��������Խ�������κβ���ӳ�䵽���������κβ��֡� �ڱ��̳��У����ڼ�ԭ����ֻ�ǽ���ֱ��ӳ�䡣*/


	//�������ݵ���������
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);


	//�������� ��Ҫ˳ʱ��
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;


	/*�������㻺������������������ ���������������ķ�ʽ��ͬ��
	������д�������������� �������У�ByteWidth���������Ĵ�С���� BindFlags�����������ͣ�����Ҫȷ����ȷ��д�����ݡ� 
	��д�����󣬻���Ҫ��дһ������Դָ�룬��ָ�뽫ָ��֮ǰ�����Ķ�����������顣 
	ʹ��˵��������Դָ�룬����ʹ�� D3D �豸���� CreateBuffer����������ָ���»�������ָ��*/

	//���þ�̬���㻺����������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Ϊ����Դ�ṹ�ṩһ��ָ�򶥵����ݵ�ָ�롣
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//�������㻺����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//���þ�̬��������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//�ṩ�������ݵ�ָ��
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//��������������
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/*�������㻺�����������������󣬿���ɾ��������������飬��Ϊ�����Ѹ��Ƶ��������У���˲�����Ҫ����*/
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;


	return true;
}

//�ͷ�������ص�ָ������
void ModelClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

/*�� GPU �����������Ͻ����㻺��������������������Ϊ�״̬�� 
һ�� GPU ���л�Ķ��㻺���������Ϳ���ʹ����ɫ������Ⱦ�û������� 
�˺���������Ӧ��λ�����Щ�����������������Ρ����������εȡ�*/

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//���ö��㻺����������ƫ��
	stride = sizeof(VertexType);
	offset = 0;

	//������������н���Щ����������Ϊ����Ա���������Ⱦ
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//���йػ�Ԫ�����Լ��������������׶��������ݵ�����˳�����Ϣ
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//���������ݽ���Ϊ�������б�

	return;
}

//LoadTexture ��һ���µ�˽�к��������������������Ȼ��ʹ���ṩ�������ļ���������г�ʼ���� �˺����ڳ�ʼ���ڼ���á�
bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

//ReleaseTexture �������ͷ��� LoadTexture �����ڼ䴴���ͼ��ص��������
void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}