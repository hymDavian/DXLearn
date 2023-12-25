#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

D3DClass::D3DClass(const D3DClass&)
{
}

D3DClass::~D3DClass()
{
}

/// <summary>
/// dx11整体设定初始化
/// </summary>
/// <param name="screenWidth">创建的窗口宽度</param>
/// <param name="screenHeight">创建的窗口高度</param>
/// <param name="vsync">是否需要根据监视器刷新呈现，否则是尽快呈现</param>
/// <param name="hwnd">窗口句柄，用以访问以前创建的窗口</param>
/// <param name="fullscreen">是否全屏</param>
/// <param name="screenDepth">渲染3D环境的深度设置</param>
/// <param name="screenNear">近景值</param>
/// <returns></returns>
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;//用于描述错误或警告的 32 位值。
	IDXGIFactory* factory;//处理全屏转换
	IDXGIAdapter* adapter;//显示子系统 (包括一个或多个 GPU、DAC 和视频内存) 
	IDXGIOutput* adapterOutput;//表示适配器输出
	unsigned int numModes, i, numerator, denominator;//模式,i,分子,分母
	unsigned long long stringLength;//字符长度
	DXGI_MODE_DESC* displayModeList;//显示模式
	DXGI_ADAPTER_DESC adapterDesc;//用 DXGI 1.0 的适配器 （或视频卡）
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;//描述交换链
	D3D_FEATURE_LEVEL featureLevel;//Direct3D 设备面向的功能集枚举
	ID3D11Texture2D* backBufferPtr;//纹理数据
	D3D11_TEXTURE2D_DESC depthBufferDesc;//纹理描述
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;//深度模板状态
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;//指定可从深度模具视图访问的纹理子资源
	D3D11_RASTERIZER_DESC rasterDesc;//光栅器状态
	float fieldOfView, screenAspect;
	m_vsync_enabled = vsync;

	/*在初始化 Direct3D 之前，我们必须从视频卡/显示器获取刷新率。 
	每台计算机可能略有不同，因此我们需要查询该信息。 
	我们查询分子和分母值，然后在设置过程中将它们传递给 DirectX，它将计算适当的刷新率。
	*/
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {//针对任何状态值失败的泛型测试
		return false;
	}

	//使用工厂为主图形接口（视频卡）创建适配器。
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {//针对任何状态值失败的泛型测试
		return false;
	}

	//枚举主适配器输出
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {//针对任何状态值失败的泛型测试
		return false;
	}

	//指定资源数据格式
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	//创建一个列表以保存此监视器/视频卡组合的所有可能显示模式。
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) 
	{
		return false;
	}

	//现在填充显示模式列表结构
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	//浏览所有显示模式，找到与屏幕宽度和高度匹配的模式
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				//存储该模式监视器刷新率的分子和分母
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//使用适配器检索显卡名称和内存
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	//以兆字节为单位存储专用视频卡内存。
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//将视频卡的名称转换为字符数组并存储。
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	/*现在我们已经存储了刷新率的分子和分母以及视频卡信息，
	我们可以发布用于获取这些信息的结构和接口。*/

	//释放显示列表
	delete[] displayModeList;
	displayModeList = 0;

	//释放交换链输出
	adapterOutput->Release();
	adapterOutput = 0;

	//释放交换链
	adapter->Release();
	adapter = 0;

	//释放工厂
	factory->Release();
	factory = 0;

	/*现在我们有了系统的刷新率，我们可以开始DirectX初始化了。
	我们要做的第一件事是填写交换链的描述。
	交换链是用于绘制图形的前缓冲区和后缓冲区。
	通常，使用单个后缓冲区，对其进行所有绘制，然后将其交换到前缓冲区，然后显示在用户屏幕上。
	这就是为什么它被称为交换链。*/

	//初始化交换链描述
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//设置为单个后台缓冲区。
	swapChainDesc.BufferCount = 1;

	//在缓冲区设置宽高
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//为后缓冲区设置常规的32位曲面。
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/*交换链描述的下一部分是刷新率。刷新率是指每秒从后缓冲区到前缓冲区的刷新次数。
	如果在applicationclass.h标头中将vsync设置为true，那么这将把刷新率锁定为系统设置（例如60hz）。
	这意味着它每秒只画60次屏幕（如果系统刷新率超过60，则会更高）。
	然而，如果我们将vsync设置为false，那么它将尽可能多地每秒绘制屏幕，然而这可能会导致一些视觉伪影。*/

	//设置后缓冲区刷新率
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//设置后缓冲区使用情况
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//使用图面或资源作为输出呈现目标。

	//设置要渲染到的窗口句柄。
	swapChainDesc.OutputWindow = hwnd;

	//关闭多重采样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//设置是否全屏还是窗口
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//将扫描线顺序和缩放设置为未指定。
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//演示后 丢弃后台缓冲区内容。
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//不设置高级标记
	swapChainDesc.Flags = 0;

	/*在设置交换链描述之后，我们还需要再设置一个称为功能级别的变量。
	此变量告诉DirectX我们计划使用的版本。在这里，我们将功能级别设置为11.0，即DirectX 11。
	如果计划支持多个版本或在低端硬件上运行，则可以将此值设置为10或9以使用较低级别的DirectX。*/

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/*现在已经填写了交换链描述和功能级别，我们可以创建交换链、Direct3D设备和Direct3D设备上下文。
	Direct3D设备和Direct3D设备上下文非常重要，它们是所有Direct3D功能的接口。
	从这一点开始，我们将对几乎所有内容使用设备和设备上下文。*/

	return false;
}

void D3DClass::Shutdown()
{
}

void D3DClass::BeginScene(float, float, float, float)
{
}

void D3DClass::EndScene()
{
}

ID3D11Device* D3DClass::GetDevice()
{
	return nullptr;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return nullptr;
}

void D3DClass::GetProjectionMatrix(XMMATRIX&)
{
}

void D3DClass::GetWorldMatrix(XMMATRIX&)
{
}

void D3DClass::GetOrthoMatrix(XMMATRIX&)
{
}

void D3DClass::GetVideoCardInfo(char*, int&)
{
}

void D3DClass::SetBackBufferRenderTarget()
{
}

void D3DClass::ResetViewport()
{
}
