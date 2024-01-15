#include "textureclass.h"

TextureClass::TextureClass()
{
	//初始化三个指针
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

/*Initialize 函数将 Direct3D 设备和 Targa 映像文件的名称作为输入。 
它首先将 Targa 数据加载到数组中。 
然后它会创建一个纹理，并以正确的格式将 Targa 数据加载到其中（Targa 图像默认是倒置的，需要反转）。 
然后，加载纹理后，它将创建纹理的资源视图，供着色器用于绘制。*/

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//首先我们调用 TextureClass：：LoadTarga32Bit 函数将 Targa 文件加载到 m_targaData 数组中
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}

	/*接下来，我们需要设置将 Targa 数据加载到的 DirectX 纹理的描述。 
	我们使用 Targa 图像数据中的高度和宽度，并将格式设置为 32 位 RGBA 纹理。
	我们将 SampleDesc 设置为默认值。 
	然后我们将 Usage 设置为 D3D11_USAGE_DEFAULT 这是性能更好的内存，我们还将在下面对此进行更多解释。 
	最后，我们将 MipLevels、BindFlags 和 MiscFlags 设置为 Mipmaped 纹理所需的设置。 
	描述完成后，我们调用 CreateTexture2D 为我们创建一个空纹理。 下一步是将 Targa 数据复制到该空纹理中。*/

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

	rowPitch = (m_width * 4) * sizeof(unsigned char);//图像数据行间距

	/*在这里，我们使用 UpdateSubresource 将 Targa 数据数组实际复制到 DirectX 纹理中。 
	如果您还记得在上一个教程中，我们使用 Map 和 Unmap 将 ModelClass 中的矩阵复制到矩阵常量缓冲区中，我们也可以在这里对纹理数据执行相同的操作。 
	事实上，使用 Map 和 Unmap 通常比使用 UpdateSubresource 快得多，但是这两种加载方法都有特定的用途，
	出于性能原因，您需要正确选择使用哪一种。 
	建议对每帧或定期重新加载的数据使用 Map 和 Unmap。 
	对于将加载一次或在加载序列期间很少加载的内容，应将 UpdateSubresource 用于内容。 
	原因是 UpdateSubresource 将数据放入获得缓存保留首选项的更高速度内存中，因为它知道您不会很快删除或重新加载它。 
	我们通过使用 D3D11_USAGE_DEFAULT 让 DirectX 知道何时使用 UpdateSubresource 进行加载。 
	Map 和 Unmap 会将数据放入不会缓存的内存位置，因为 DirectX 预计该数据很快就会被覆盖。 
	这就是为什么我们使用 D3D11_USAGE_DYNAMIC 来通知 DirectX 这种类型的数据是暂时性的。*/

	//拷贝图像数据到纹理
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	/*加载纹理后，我们创建一个着色器资源视图，该视图允许我们有一个指针来设置着色器中的纹理。 
	在描述中，我们还设置了两个重要的 Mipmap 变量，它们将为我们提供全方位的 Mipmap 级别，以便在任何距离上进行高质量的纹理渲染。 
	创建着色器资源视图后，我们调用 GenerateMips，它会为我们创建 Mipmap，
	但是，如果您愿意，如果您正在寻找更好的质量，您可以手动加载自己的 Mipmap 级别。*/

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	//生成纹理映射
	deviceContext->GenerateMips(m_textureView);

	//现在图像数据已经加载到纹理中，释放targa图像数据
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

/*这是我们的 Targa 图像加载功能。 
再次注意，Targa 图像是倒置存储的，使用前需要翻转。 
所以在这里我们将打开文件，将其读入数组，然后获取该数组数据并以正确的顺序将其加载到 m_targaData 数组中。 
请注意，我们特意只处理具有 alpha 通道的 32 位 Targa 文件，此函数将拒绝保存为 24 位的 Targa。*/

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
