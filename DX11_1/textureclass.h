#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <stdio.h>

class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	int GetWidth();
	int GetHeight();

private:
	//这里有我们的Targa读取功能。如果你想支持更多的格式，你可以在这里添加阅读功能。

	bool LoadTarga32Bit(char*);

private:
	/*这个类有五个成员变量。
	第一个保存直接从文件中读取的原始Targa数据。
	第二个变量m_texture将保存DirectX将用于渲染的结构化纹理数据。
	第三个变量是着色器在绘制时用于访问纹理数据的资源视图。
	宽度和高度是纹理的尺寸。*/

	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	int m_width, m_height;
};

#endif // !_TEXTURECLASS_H_


