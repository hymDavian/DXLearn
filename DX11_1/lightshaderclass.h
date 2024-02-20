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
	//�µ� LightBufferType �ṹ�����ڱ���������Ϣ�� �� typedef ��������ɫ���е��� typedef ��ͬ�� ��ע�⣬��Ϊ��С��������һ������ĸ���������ȷ���ṹ�� 16 �ı����� 
	// ����û�ж��⸡�����Ľṹֻ�� 28 ���ֽڣ�����������ʹ�� sizeof��LightBufferType����CreateBuffer �ͻ�ʧ�ܣ���Ϊ����Ҫ��С�� 16 �ı������ܳɹ���

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;  // ����˶������䣬��˽ṹ��CreateBuffer����Ҫ���16�ı�����
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
	//��һ���µ�ר�ó�����������������ʾ������Ϣ����ɫ�ͷ��򣩡� ���ཫʹ�ù⻺���������� HLSL ������ɫ���е�ȫ�ֹ������

	ID3D11Buffer* m_lightBuffer;
};

#endif