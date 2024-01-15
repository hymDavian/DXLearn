#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "textureclass.h"

class ModelClass
{
private:
	//�������͵Ķ��壬������� ModelClass �еĶ��㻺����һ��ʹ�á� ����ע�⣬�� typedef ������ ColorShaderClass �еĲ���ƥ��
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	/*����ģ�͵Ķ���������������ĳ�ʼ���͹رա� Render ������ģ�ͼ���ͼ�η�����Ƶ����*/

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	//ModelClass ���ھ��� GetTexture ��������������Խ��Լ���������Դ���ݸ������ƴ�ģ�͵���ɫ����
	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// LoadTexture �� ReleaseTexture�����ڼ��غ��ͷŽ�������Ⱦ��ģ�͵�����

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	//���㻺����
	ID3D11Buffer* m_vertexBuffer;
	//����������
	ID3D11Buffer* m_indexBuffer;

	int m_vertexCount;

	int m_indexCount;

	//������Դ
	TextureClass* m_Texture;

};

#endif // !_MODELCLASS_H_

