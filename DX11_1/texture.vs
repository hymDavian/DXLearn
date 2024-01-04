
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

/*不再在顶点类型中使用颜色，而是改用纹理坐标。 由于纹理坐标采用 U 和 V 浮点坐标，因此使用 float2 作为其类型。 
纹理坐标的语义TEXCOORD0顶点着色器和像素着色器。 
可以将零更改为任意数字，以指示正在使用的坐标集，因为允许使用多个纹理坐标。*/

//输入的顶点类型，之后在c++层的shader类的输入布局里需要定义这俩个参数描述
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