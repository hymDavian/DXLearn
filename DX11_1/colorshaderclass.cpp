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

//调用着色器的初始化函数。 传入 HLSL 着色器文件的名称
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];//顶点着色器源码文件地址
	wchar_t psFilename[128];//像素着色器源码文件地址
	int error;

	//设置顶点着色器文件
	//wcscpy_s 拷贝宽字符串的安全函数 返回 error_t 类型值
	error = wcscpy_s(vsFilename, 128, L"../DX11_1/color.vs");
	if (error != 0)
	{
		return false;
	}
	//设置像素着色器文件
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

//Render 将首先使用 SetShaderParameters 函数在着色器中设置参数。 设置参数后，它会调用 RenderShader 以使用 HLSL 着色器绘制绿色三角形。

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

/* InitializeShader 加载着色器文件并使其可用于 DirectX 和 GPU。 
布局的设置以及顶点缓冲区数据在 GPU 中图形管道上的外观。 
布局需要匹配 modelclass.h 文件中的 VertexType 以及 color.vs 文件中定义的 VertexType。*/
//初始化shader
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	//ID3D10Blob 返回任意长度的数据

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];//输入装配器阶段的单个元素的说明
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;//矩阵缓冲区描述

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	/*在这里，将着色器程序编译为缓冲区。 
	提供着色器文件的名称、着色器的名称、着色器版本（DirectX 11 中的 5.0）以及用于编译着色器的缓冲区。 
	如果编译着色器失败，它将在 errorMessage 字符串中放置一条错误消息，将该字符串发送到另一个函数以写出错误。 
	如果它仍然失败并且没有 errorMessage 字符串，则表示它找不到着色器文件，在这种情况下，弹出一个对话框来说明这一点。*/

	//编译顶点着色器代码 下面依次是参数含义
	//源码文件地址，
	// 定义着色器宏 的可选D3D_SHADER_MACRO 结构数组，
	// 指向 ID3DInclude 接口的可选指针，编译器使用该指针来处理包含文件，
	// 着色器执行开始处的着色器入口点函数的名称
	// 要编译的着色器目标，【..】表示顶点着色器
	// 指定编译器如何编译 HLSL 代码,
	// 生成的值指定编译器如何编译效果，如果是编译着色器，会忽略，建议设为0
	// 接收指向可用于访问已编译代码的指针
	// 接收指向可用于访问编译器错误消息的指针
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//如果着色器未能编译，它应该已经向错误消息写入了一些内容。
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else //如果错误消息中没有任何内容，那么它就是找不到着色器文件本身。
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//编译像素着色器
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

	/*一旦顶点着色器和像素着色器代码成功编译到缓冲区中，
	使用这些缓冲区来创建着色器对象本身。 从现在开始，将使用这些指针与顶点和像素着色器进行交互。*/

	//从缓冲区创建顶点着色器对象
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//像素着色器
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*下一步是创建将由着色器处理的顶点数据的布局。 由于此着色器使用位置和颜色矢量，因此需要在布局中创建两者，指定两者的大小。 
	语义名称是布局中首先要填写的内容，这允许着色器确定布局中此元素的用法。
	由于有两个不同的元素，使用 POSITION 作为第一个元素，将 COLOR 用于第二个元素。
	布局的下一个重要部分是格式。 对于位置向量，使用 DXGI_FORMAT_R32G32B32_FLOAT，对于颜色，使用 DXGI_FORMAT_R32G32B32A32_FLOAT。 
	需要注意的最后一件事是 AlignedByteOffset，它指示数据在缓冲区中的间隔方式。 
	对于此布局，前 12 个字节是位置，接下来的 16 个字节将是颜色，AlignedByteOffset 显示每个元素的开始位置。
	可以使用 D3D11_APPEND_ALIGNED_ELEMENT 而不是将自己的值放在 AlignedByteOffset 中，它会计算出间距*/

	//创建顶点输入布局描述
	//此设置需要匹配ModelClass和着色器中的VertexType结构
	polygonLayout[0].SemanticName = "POSITIONT";//着色器输入签名中与此元素关联的 HLSL 语义
	polygonLayout[0].SemanticIndex = 0;//元素的语义索引。 语义索引使用整数索引号修改语义。 仅当有多个具有相同语义的元素时，才需要语义索引。 例如，4x4 矩阵有四个组件，每个组件都有语义名称
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;//元素数据的数据类型,【】一种三分量 96 位浮点格式
	polygonLayout[0].InputSlot = 0;//标识输入装配器 (输入槽) 的整数值。 有效值介于 0 和 15 之间
	polygonLayout[0].AlignedByteOffset = 0;//当前元素值偏移
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//标识单个输入槽的输入数据类，【】输入数据是每个顶点数据。
	polygonLayout[0].InstanceDataStepRate = 0;//在缓冲区中向前推进一个元素之前，使用相同每个实例数据绘制的实例数。 对于包含每个顶点数据的元素，此值必须为 0

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
