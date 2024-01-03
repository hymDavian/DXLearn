#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;
	//初始化顶点和索引缓冲区
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	//调用缓冲区关闭
	ShutdownBuffers();
	return;
}

/*Render 是从 ApplicationClass：：Render 函数调用的。 
此函数调用 RenderBuffers 将顶点和索引缓冲区放在图形管道上，以便颜色着色器能够呈现它们。*/

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

//返回模型中的索引数。颜色着色器将需要此信息来绘制此模型
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

/*处理创建顶点和索引缓冲区,通常，会读入模型并从该数据文件创建缓冲区*/

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	/*首先创建两个临时数组来保存顶点和索引数据，
	稍后将使用这些数组来填充最终缓冲区。*/

	//设置顶点数组的数量 
	//以三角形为例
	m_vertexCount = 3;

	//设置索引数组的数量
	m_indexCount = 3;

	//创建顶点数组
	vertices = new VertexType[m_vertexCount];
	if (!vertices) { return false; }

	//创建索引数组
	indices = new unsigned long[m_indexCount];
	if (!indices) { return false; }

	/*现在用三角形的三个点以及每个点的索引填充顶点和索引数组。 
	请注意，按照绘制点的顺时针顺序创建点。 如果逆时针这样做，它会认为三角形朝向相反的方向，并且由于背面剔除而没有绘制它。
	将顶点发送到 GPU 的顺序非常重要。 颜色也在此处设置，因为它是顶点描述的一部分。*/

	//载入数据到顶点数组
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);//左边点
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);//绿色
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);//中点
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);//右点
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//索引数据 需要顺时针
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	/*创建顶点缓冲区和索引缓冲区。 创建两个缓冲区的方式相同。
	首先填写缓冲区的描述。 在描述中，ByteWidth（缓冲区的大小）和 BindFlags（缓冲区类型）是需要确保正确填写的内容。 
	填写描述后，还需要填写一个子资源指针，该指针将指向之前创建的顶点或索引数组。 
	使用说明和子资源指针，可以使用 D3D 设备调用 CreateBuffer，它将返回指向新缓冲区的指针*/

	//设置静态顶点缓冲区的描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//为子资源结构提供一个指向顶点数据的指针。
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//创建顶点缓冲区
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//设置静态索引缓冲区描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//提供索引数据的指针
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//创建索引缓冲区
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/*创建顶点缓冲区和索引缓冲区后，可以删除顶点和索引数组，因为数据已复制到缓冲区中，因此不再需要它们*/
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;


	return true;
}

//释放自身相关的指针数据
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

/*在 GPU 的输入汇编器上将顶点缓冲区和索引缓冲区设置为活动状态。 
一旦 GPU 具有活动的顶点缓冲区，它就可以使用着色器来渲染该缓冲区。 
此函数还定义应如何绘制这些缓冲区，例如三角形、线条、扇形等。*/

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//设置顶点缓冲区步长和偏移
	stride = sizeof(VertexType);
	offset = 0;

	//在输入汇编程序中将这些缓冲区设置为活动，以便对其进行渲染
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//绑定有关基元类型以及描述输入汇编程序阶段输入数据的数据顺序的信息
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//将顶点数据解释为三角形列表

	return;
}
