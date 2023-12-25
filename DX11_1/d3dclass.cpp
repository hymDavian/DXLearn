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
	D3D11_TEXTURE2D_DESC depthBufferDesc;//��������
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
