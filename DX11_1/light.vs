/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};
//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
    

    // 将位置矢量更改为4个单位，以便进行正确的矩阵计算。
	input.position.w = 1.0f;

    // 根据世界、视图和投影矩阵计算顶点的位置。
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
    
    // 存储像素着色器的纹理坐标。
	output.tex = input.tex;
	//此顶点的法向量在世界空间中计算， 然后归一化，
	//然后作为输入发送到像素着色器。 我们只根据世界矩阵进行计算，
	//因为我们只是想找到3 D
	//世界空间中的照明值。 请注意，
	//有时由于发生的插值， 这些法线需要在像素着色器内重新归一化。

    // 仅根据世界矩阵计算法向量。
	output.normal = mul(input.normal, (float3x3) worldMatrix);
	
    //对法线向量进行规格化
	output.normal = normalize(output.normal);

	return output;
}