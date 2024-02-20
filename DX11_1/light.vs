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
    

    // ��λ��ʸ������Ϊ4����λ���Ա������ȷ�ľ�����㡣
	input.position.w = 1.0f;

    // �������硢��ͼ��ͶӰ������㶥���λ�á�
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
    
    // �洢������ɫ�����������ꡣ
	output.tex = input.tex;
	//�˶���ķ�����������ռ��м��㣬 Ȼ���һ����
	//Ȼ����Ϊ���뷢�͵�������ɫ���� ����ֻ�������������м��㣬
	//��Ϊ����ֻ�����ҵ�3 D
	//����ռ��е�����ֵ�� ��ע�⣬
	//��ʱ���ڷ����Ĳ�ֵ�� ��Щ������Ҫ��������ɫ�������¹�һ����

    // ���������������㷨������
	output.normal = mul(input.normal, (float3x3) worldMatrix);
	
    //�Է����������й��
	output.normal = normalize(output.normal);

	return output;
}