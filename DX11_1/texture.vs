
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

/*�����ڶ���������ʹ����ɫ�����Ǹ����������ꡣ ��������������� U �� V �������꣬���ʹ�� float2 ��Ϊ�����͡� 
�������������TEXCOORD0������ɫ����������ɫ���� 
���Խ������Ϊ�������֣���ָʾ����ʹ�õ����꼯����Ϊ����ʹ�ö���������ꡣ*/

//����Ķ������ͣ�֮����c++���shader������벼������Ҫ������������������
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position:SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    
    return output;

}