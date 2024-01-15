#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "textureclass.h"

class ModelClass
{
private:
	//顶点类型的定义，它将与此 ModelClass 中的顶点缓冲区一起使用。 另请注意，此 typedef 必须与 ColorShaderClass 中的布局匹配
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	/*处理模型的顶点和索引缓冲区的初始化和关闭。 Render 函数将模型几何图形放在视频卡上*/

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	//ModelClass 现在具有 GetTexture 函数，因此它可以将自己的纹理资源传递给将绘制此模型的着色器。
	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// LoadTexture 和 ReleaseTexture，用于加载和释放将用于渲染此模型的纹理。

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	//顶点缓冲区
	ID3D11Buffer* m_vertexBuffer;
	//索引缓冲区
	ID3D11Buffer* m_indexBuffer;

	int m_vertexCount;

	int m_indexCount;

	//纹理资源
	TextureClass* m_Texture;

};

#endif // !_MODELCLASS_H_

