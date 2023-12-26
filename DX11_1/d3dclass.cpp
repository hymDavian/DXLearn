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
/// dx11�����趨��ʼ��
/// </summary>
/// <param name="screenWidth">�����Ĵ��ڿ��</param>
/// <param name="screenHeight">�����Ĵ��ڸ߶�</param>
/// <param name="vsync">�Ƿ���Ҫ���ݼ�����ˢ�³��֣������Ǿ������</param>
/// <param name="hwnd">���ھ�������Է�����ǰ�����Ĵ���</param>
/// <param name="fullscreen">�Ƿ�ȫ��</param>
/// <param name="screenDepth">��Ⱦ3D�������������</param>
/// <param name="screenNear">����ֵ</param>
/// <returns></returns>
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;//������������򾯸�� 32 λֵ��
	IDXGIFactory* factory;//����ȫ��ת��
	IDXGIAdapter* adapter;//��ʾ��ϵͳ (����һ������ GPU��DAC ����Ƶ�ڴ�) 
	IDXGIOutput* adapterOutput;//��ʾ���������
	unsigned int numModes, i, numerator, denominator;//ģʽ,i,����,��ĸ
	unsigned long long stringLength;//�ַ�����
	DXGI_MODE_DESC* displayModeList;//��ʾģʽ
	DXGI_ADAPTER_DESC adapterDesc;//�� DXGI 1.0 �������� ������Ƶ����
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;//����������
	D3D_FEATURE_LEVEL featureLevel;//Direct3D �豸����Ĺ��ܼ�ö��
	ID3D11Texture2D* backBufferPtr;//��������
	D3D11_TEXTURE2D_DESC depthBufferDesc;//�����������
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;//���ģ��״̬
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;//ָ���ɴ����ģ����ͼ���ʵ���������Դ
	D3D11_RASTERIZER_DESC rasterDesc;//��դ��״̬
	float fieldOfView, screenAspect;
	m_vsync_enabled = vsync;

	/*�ڳ�ʼ�� Direct3D ֮ǰ�����Ǳ������Ƶ��/��ʾ����ȡˢ���ʡ� 
	ÿ̨������������в�ͬ�����������Ҫ��ѯ����Ϣ�� 
	���ǲ�ѯ���Ӻͷ�ĸֵ��Ȼ�������ù����н����Ǵ��ݸ� DirectX�����������ʵ���ˢ���ʡ�
	*/
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {//����κ�״ֵ̬ʧ�ܵķ��Ͳ���
		return false;
	}

	//ʹ�ù���Ϊ��ͼ�νӿڣ���Ƶ����������������
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {//����κ�״ֵ̬ʧ�ܵķ��Ͳ���
		return false;
	}

	//ö�������������
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {//����κ�״ֵ̬ʧ�ܵķ��Ͳ���
		return false;
	}

	//ָ����Դ���ݸ�ʽ
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	//����һ���б��Ա���˼�����/��Ƶ����ϵ����п�����ʾģʽ��
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) 
	{
		return false;
	}

	//���������ʾģʽ�б�ṹ
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	//���������ʾģʽ���ҵ�����Ļ��Ⱥ͸߶�ƥ���ģʽ
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				//�洢��ģʽ������ˢ���ʵķ��Ӻͷ�ĸ
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//ʹ�������������Կ����ƺ��ڴ�
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	//�����ֽ�Ϊ��λ�洢ר����Ƶ���ڴ档
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//����Ƶ��������ת��Ϊ�ַ����鲢�洢��
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	/*���������Ѿ��洢��ˢ���ʵķ��Ӻͷ�ĸ�Լ���Ƶ����Ϣ��
	���ǿ��Է������ڻ�ȡ��Щ��Ϣ�Ľṹ�ͽӿڡ�*/

	//�ͷ���ʾ�б�
	delete[] displayModeList;
	displayModeList = 0;

	//�ͷŽ��������
	adapterOutput->Release();
	adapterOutput = 0;

	//�ͷŽ�����
	adapter->Release();
	adapter = 0;

	//�ͷŹ���
	factory->Release();
	factory = 0;

	/*������������ϵͳ��ˢ���ʣ����ǿ��Կ�ʼDirectX��ʼ���ˡ�
	����Ҫ���ĵ�һ��������д��������������
	�����������ڻ���ͼ�ε�ǰ�������ͺ󻺳�����
	ͨ����ʹ�õ����󻺳���������������л��ƣ�Ȼ���佻����ǰ��������Ȼ����ʾ���û���Ļ�ϡ�
	�����Ϊʲô������Ϊ��������*/

	//��ʼ������������
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//����Ϊ������̨��������
	swapChainDesc.BufferCount = 1;

	//�ڻ��������ÿ��
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//Ϊ�󻺳������ó����32λ���档
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/*��������������һ������ˢ���ʡ�ˢ������ָÿ��Ӻ󻺳�����ǰ��������ˢ�´�����
	�����applicationclass.h��ͷ�н�vsync����Ϊtrue����ô�⽫��ˢ��������Ϊϵͳ���ã�����60hz����
	����ζ����ÿ��ֻ��60����Ļ�����ϵͳˢ���ʳ���60�������ߣ���
	Ȼ����������ǽ�vsync����Ϊfalse����ô���������ܶ��ÿ�������Ļ��Ȼ������ܻᵼ��һЩ�Ӿ�αӰ��*/

	//���ú󻺳���ˢ����
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

	//���ú󻺳���ʹ�����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//ʹ��ͼ�����Դ��Ϊ�������Ŀ�ꡣ

	//����Ҫ��Ⱦ���Ĵ��ھ����
	swapChainDesc.OutputWindow = hwnd;

	//�رն��ز���
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//�����Ƿ�ȫ�����Ǵ���
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//��ɨ����˳�����������Ϊδָ����
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//��ʾ�� ������̨���������ݡ�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�����ø߼����
	swapChainDesc.Flags = 0;

	/*�����ý���������֮�����ǻ���Ҫ������һ����Ϊ���ܼ���ı�����
	�˱�������DirectX���Ǽƻ�ʹ�õİ汾����������ǽ����ܼ�������Ϊ11.0����DirectX 11��
	����ƻ�֧�ֶ���汾���ڵͶ�Ӳ�������У�����Խ���ֵ����Ϊ10��9��ʹ�ýϵͼ����DirectX��*/

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/*�����Ѿ���д�˽����������͹��ܼ������ǿ��Դ�����������Direct3D�豸��Direct3D�豸�����ġ�
	Direct3D�豸��Direct3D�豸�����ķǳ���Ҫ������������Direct3D���ܵĽӿڡ�
	����һ�㿪ʼ�����ǽ��Լ�����������ʹ���豸���豸�����ġ�*/

	//������ʾ��ʾ���������豸�Լ����ڳ��ֵĽ�������
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	/*��ʱ���������Ƶ����DirectX 11�����ݣ��򴴽��豸�ĵ��ý�ʧ�ܡ�
	ĳЩ�������ܽ�������ΪDirectX 10��Ƶ�������ο���ΪDirectX 11��Ƶ����
	���⣬һЩ����Կ�Ҳ�����������ģ���Ҫ�ǵ͹��ĵ�Ӣ�ض�������Ҫ�Ǹ߹��ĵ�Ӣΰ�￨��
	Ҫ��������⣬��Ҫ��ʹ��Ĭ���豸������ö�ٻ����е�������Ƶ���������û�ѡ��Ҫʹ�õ���Ƶ����
	Ȼ���ڴ����豸ʱָ���ÿ���*/
	/*�����������˽�������������Ҫ���һ��ָ��󻺳�����ָ�룬Ȼ�������ӵ���������
	���ǽ�ʹ��CreateRenderTargetView��������̨���������ӵ���������*/

	//��ȡָ��󻺳�����ָ��
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//ʹ�ú󻺳���ָ�봴����ȾĿ����ͼ��
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	//�ͷ�ָ��󻺳�����ָ�룬��Ϊ���ǲ�����Ҫ����
	backBufferPtr->Release();
	backBufferPtr = 0;

	/*����Ҫ����һ����Ȼ�������������ʹ������������Ȼ��������Ա���3D�ռ�����ȷ��Ⱦ����Ρ�
	ͬʱ��������Ȼ������и���һ��ģ�滺������ģ�߻�����������ʵ���˶�ģ���������Ӱ��Ч����*/

	//��ʼ����Ȼ�����������
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//������Ȼ���������
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//32λz��������ʽ��֧��24λ��Ⱥ�8λģ��
	depthBufferDesc.SampleDesc.Count = 1;//Ϊ����ָ�����ز��������Ľṹ��ÿ�����صĶ�������Ϊ1
	depthBufferDesc.SampleDesc.Quality = 0;  //ͼ���������� ����Խ�ߣ�����Խ��.��Ч��Χ���� 0 �� 1 ֮��
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;//��ʶ��ζ�ȡ��д�������ֵ.��D3D11_USAGE_DEFAULT����ʾ��Ҫ GPU �Ķ�ȡ��д�����Ȩ�޵���Դ
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;//��ʶ��ν���Դ�󶨵��ܵ�,��D3D11_BIND_DEPTH_STENCIL���������Ϊ����ϲ��׶ε����ģ��Ŀ�ꡣ
	depthBufferDesc.CPUAccessFlags = 0;//��ָ������� CPU �������͵ı�־,0��ʾ����ҪCPU����
	
	/* ����������������ͼ������ D3D11_RESOURCE_MISC_TEXTURECUBE ��־�� 
	��ά���ݼ�ӳ������ (�� ArraySize> 6) ��Ҫ ���ܼ���D3D_FEATURE_LEVEL_10_1 ����ߡ�*/
	depthBufferDesc.MiscFlags = 0;//��־

	/*ʹ�ø������������/ģ�߻�������
	ʹ��CreateTexture2D��������������������˻�����ֻ��һ��2D����
	��������ԭ���ǣ�һ���Զ���ν������򲢹�դ�����������վͻ��Ϊ2D�������еĲ�ɫ���ء�Ȼ�����2D�����������Ƶ���Ļ��*/
	//ʹ����������Ϊ��Ȼ�������������
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/*������Ҫ�������ģ�����������ڿ���Direct3D����ÿ�����ؽ��к������͵���Ȳ��ԡ�*/
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;//������Ȳ���
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//����д��������ݵ����ģ�߻���������,��D3D11_DEPTH_WRITE_MASK_ALL����ʾ���ö����ģ�߻�������д��
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//���ڽ��������������������ݽ��бȽϵĺ�������D3D11_COMPARISON_LESS��ָʾ��Сֵ��ǰ
	depthStencilDesc.StencilEnable = true;//�������߲���
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	//���������ؽ��е�ģ�߲���
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//ģ�߲���ʧ��ʱҪִ�е�ģ�߲���,��..����������ģ�����ݡ�
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;//ģ�߲���ͨ������Ȳ���ʧ��ʱҪִ�е�ģ�߲�����..����ģ��ֵ���� 1�����б�Ҫ�����������
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//ģ�߲��Ժ���Ȳ��Զ�ͨ��ʱҪִ�е�ģ�߲���,��..��
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//��ģ������������ģ�����ݽ��бȽϣ���..��ʼ�մ��ݱȽ�


	return true;
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
