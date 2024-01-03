#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class ModelClass
{
private:
	//顶点类型的定义，它将与此 ModelClass 中的顶点缓冲区一起使用。 另请注意，此 typedef 必须与 ColorShaderClass 中的布局匹配
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	/*处理模型的顶点和索引缓冲区的初始化和关闭。 Render 函数将模型几何图形放在视频卡上*/

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	//顶点缓冲区
	ID3D11Buffer* m_vertexBuffer;
	//索引缓冲区
	ID3D11Buffer* m_indexBuffer;

	int m_vertexCount;

	int m_indexCount;

};

#endif // !_MODELCLASS_H_

