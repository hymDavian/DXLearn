/*����ɫ�������У���ȫ�ֱ�����ʼ�� ���Դ� C++ �������ⲿ�޸���Щȫ�ֱ����� 
����ʹ�ö������͵ı��������� int �� float��Ȼ�����ⲿ���������Թ���ɫ������ʹ�á� 
ͨ�����������ȫ�ֱ������ڳ�Ϊ��cbuffer���Ļ��������������У���ʹ��ֻ��һ��ȫ�ֱ����� 
���߼�����֯��Щ������������ɫ���ĸ�Чִ���Լ��Կ���δ洢�������ǳ���Ҫ�� 

���ｫ�����������ͬһ���������У���Ϊ����ÿһ֡��ͬʱ�������ǡ�*/
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//TYPEDEFS
//������������
struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

//������������
struct PixelInputType
{
	float4 position:SV_POSITION;
    float4 color : COLOR;
};

//������ɫ���Ĵ���������GPU���ã��ᴫ�� ���㻺�����е����ݸ�ʽ �������Ķ���������ݸ�ʽ����תΪ���涨��� VertexInputType ����
//Ȼ������������᷵��һ��������Ϣ�������Ϣ�ᷢ�͸�������ɫ��

//������ɫ��
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    //��λ��ʸ������Ϊ4����λ���Ա������ȷ�ľ�����㡣
    input.position.w = 1.0f;
    
    //���γ������硢��ͼ��ͶӰ�������ڼ��㶥������Ļ�ϵ���Ⱦλ��
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.color = input.color;
    //output.color.a *= 0.5f;
    
    return output;
}