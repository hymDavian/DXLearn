#include "textureclass.h"

TextureClass::TextureClass()
{
	//��ʼ������ָ��
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass&)
{
}

TextureClass::~TextureClass()
{
}

/*Initialize ������ Direct3D �豸�� Targa ӳ���ļ���������Ϊ���롣 
�����Ƚ� Targa ���ݼ��ص������С� 
Ȼ�����ᴴ��һ������������ȷ�ĸ�ʽ�� Targa ���ݼ��ص����У�Targa ͼ��Ĭ���ǵ��õģ���Ҫ��ת���� 
Ȼ�󣬼�����������������������Դ��ͼ������ɫ�����ڻ��ơ�*/

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//�������ǵ��� TextureClass����LoadTarga32Bit ������ Targa �ļ����ص� m_targaData ������
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}

	/*��������������Ҫ���ý� Targa ���ݼ��ص��� DirectX ����������� 
	����ʹ�� Targa ͼ�������еĸ߶ȺͿ�ȣ�������ʽ����Ϊ 32 λ RGBA ����
	���ǽ� SampleDesc ����ΪĬ��ֵ�� 
	Ȼ�����ǽ� Usage ����Ϊ D3D11_USAGE_DEFAULT �������ܸ��õ��ڴ棬���ǻ���������Դ˽��и�����͡� 
	������ǽ� MipLevels��BindFlags �� MiscFlags ����Ϊ Mipmaped ������������á� 
	������ɺ����ǵ��� CreateTexture2D Ϊ���Ǵ���һ�������� ��һ���ǽ� Targa ���ݸ��Ƶ��ÿ������С�*/

	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	rowPitch = (m_width * 4) * sizeof(unsigned char);//ͼ�������м��

	/*���������ʹ�� UpdateSubresource �� Targa ��������ʵ�ʸ��Ƶ� DirectX �����С� 
	��������ǵ�����һ���̳��У�����ʹ�� Map �� Unmap �� ModelClass �еľ����Ƶ��������������У�����Ҳ�������������������ִ����ͬ�Ĳ����� 
	��ʵ�ϣ�ʹ�� Map �� Unmap ͨ����ʹ�� UpdateSubresource ��ö࣬���������ּ��ط��������ض�����;��
	��������ԭ������Ҫ��ȷѡ��ʹ����һ�֡� 
	�����ÿ֡�������¼��ص�����ʹ�� Map �� Unmap�� 
	���ڽ�����һ�λ��ڼ��������ڼ���ټ��ص����ݣ�Ӧ�� UpdateSubresource �������ݡ� 
	ԭ���� UpdateSubresource �����ݷ����û��汣����ѡ��ĸ����ٶ��ڴ��У���Ϊ��֪��������ܿ�ɾ�������¼������� 
	����ͨ��ʹ�� D3D11_USAGE_DEFAULT �� DirectX ֪����ʱʹ�� UpdateSubresource ���м��ء� 
	Map �� Unmap �Ὣ���ݷ��벻�Ỻ����ڴ�λ�ã���Ϊ DirectX Ԥ�Ƹ����ݺܿ�ͻᱻ���ǡ� 
	�����Ϊʲô����ʹ�� D3D11_USAGE_DYNAMIC ��֪ͨ DirectX �������͵���������ʱ�Եġ�*/

	//����ͼ�����ݵ�����
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	/*������������Ǵ���һ����ɫ����Դ��ͼ������ͼ����������һ��ָ����������ɫ���е����� 
	�������У����ǻ�������������Ҫ�� Mipmap ���������ǽ�Ϊ�����ṩȫ��λ�� Mipmap �����Ա����κξ����Ͻ��и�������������Ⱦ�� 
	������ɫ����Դ��ͼ�����ǵ��� GenerateMips������Ϊ���Ǵ��� Mipmap��
	���ǣ������Ը�⣬���������Ѱ�Ҹ��õ��������������ֶ������Լ��� Mipmap ����*/

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	//��������ӳ��
	deviceContext->GenerateMips(m_textureView);

	//����ͼ�������Ѿ����ص������У��ͷ�targaͼ������
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

int TextureClass::GetWidth()
{
	return m_width;
}

int TextureClass::GetHeight()
{
	return m_height;
}

/*�������ǵ� Targa ͼ����ع��ܡ� 
�ٴ�ע�⣬Targa ͼ���ǵ��ô洢�ģ�ʹ��ǰ��Ҫ��ת�� 
�������������ǽ����ļ�������������飬Ȼ���ȡ���������ݲ�����ȷ��˳������ص� m_targaData �����С� 
��ע�⣬��������ֻ������� alpha ͨ���� 32 λ Targa �ļ����˺������ܾ�����Ϊ 24 λ�� Targa��*/

bool TextureClass::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	if (bpp != 32)
	{
		return false;
	}

	imageSize = m_width * m_height * 4;
	targaImage = new unsigned char[imageSize];

	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	m_targaData = new unsigned char[imageSize];
	index = 0;

	k = (m_width * m_height * 4) - (m_width * 4);
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			k += 4;
			index += 4;
		}

		k -= (m_width * 8);
	}

	delete[] targaImage;
	targaImage = 0;
	return true;
}
