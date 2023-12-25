#ifndef _D3DCLASS_H
#define _D3DCLASS_H

//Linking 链接到所需要的库
#pragma comment(lib,"d3d11.lib") //在dx3中设置和绘制3D图形的所有DX11D 功能
#pragma comment(lib,"dxgi.lib")  //获取计算机硬件接口信息
#pragma comment(lib,"d3dcompiler.lib")  //编译着色器功能

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

	//获取设备
	ID3D11Device* GetDevice();
	//获取设备上下文
	ID3D11DeviceContext* GetDeviceContext();

	//获取投影矩阵
	void GetProjectionMatrix(XMMATRIX&);//描述在映射到四个硬件矢量寄存器的 16 字节边界上对齐的 4*4 矩阵。
	//获取世界矩阵
	void GetWorldMatrix(XMMATRIX&);
	//获取正交矩阵
	void GetOrthoMatrix(XMMATRIX&);
	//获取显卡信息
	void GetVideoCardInfo(char*, int&);
	//设置缓冲区渲染目标
	void SetBackBufferRenderTarget();
	//重置视口
	void ResetViewport();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	//交换链 用于存储呈现的数据，然后再将其呈现给输出
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	//深度模具缓冲区
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	//光栅器状态
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
	D3D11_VIEWPORT m_viewport;

};


#endif // !_D3DCLASS_H
