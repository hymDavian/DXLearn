#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	//新的 LightBufferType 结构将用于保存照明信息。 此 typedef 与像素着色器中的新 typedef 相同。 请注意，我为大小填充添加了一个额外的浮点数，以确保结构是 16 的倍数。 
	// 由于没有额外浮点数的结构只有 28 个字节，因此如果我们使用 sizeof（LightBufferType），CreateBuffer 就会失败，因为它需要大小是 16 的倍数才能成功。

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;  // 添加了额外的填充，因此结构是CreateBuffer函数要求的16的倍数。
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	//有一个新的专用常量缓冲区，用于显示光线信息（颜色和方向）。 此类将使用光缓冲区来设置 HLSL 像素着色器中的全局光变量。

	ID3D11Buffer* m_lightBuffer;
};

#endif