#ifndef _D3DCLASS_H
#define _D3DCLASS_H

//Linking ���ӵ�����Ҫ�Ŀ�
#pragma comment(lib,"d3d11.lib") //��dx3�����úͻ���3Dͼ�ε�����DX11D ����
#pragma comment(lib,"dxgi.lib")  //��ȡ�����Ӳ���ӿ���Ϣ
#pragma comment(lib,"d3dcompiler.lib")  //������ɫ������

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class D3DClass 
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	//��ȡ�豸
	ID3D11Device* GetDevice();
	//��ȡ�豸������
	ID3D11DeviceContext* GetDeviceContext();

	//��ȡͶӰ����
	void GetProjectionMatrix(XMMATRIX&);//������ӳ�䵽�ĸ�Ӳ��ʸ���Ĵ����� 16 �ֽڱ߽��϶���� 4*4 ����
	//��ȡ�������
	void GetWorldMatrix(XMMATRIX&);
	//��ȡ��������
	void GetOrthoMatrix(XMMATRIX&);
	//��ȡ�Կ���Ϣ
	void GetVideoCardInfo(char*, int&);
	//���û�������ȾĿ��
	void SetBackBufferRenderTarget();
	//�����ӿ�
	void ResetViewport();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	//������ ���ڴ洢���ֵ����ݣ�Ȼ���ٽ�����ָ����
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	//���ģ�߻�����
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	//��դ��״̬
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
	D3D11_VIEWPORT m_viewport;

};


#endif // !_D3DCLASS_H
