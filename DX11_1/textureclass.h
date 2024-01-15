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
	//���������ǵ�Targa��ȡ���ܡ��������֧�ָ���ĸ�ʽ�����������������Ķ����ܡ�

	bool LoadTarga32Bit(char*);

private:
	/*������������Ա������
	��һ������ֱ�Ӵ��ļ��ж�ȡ��ԭʼTarga���ݡ�
	�ڶ�������m_texture������DirectX��������Ⱦ�Ľṹ���������ݡ�
	��������������ɫ���ڻ���ʱ���ڷ����������ݵ���Դ��ͼ��
	��Ⱥ͸߶�������ĳߴ硣*/

	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	int m_width, m_height;
};

#endif // !_TEXTURECLASS_H_


