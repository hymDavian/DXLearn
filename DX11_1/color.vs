/*在着色器程序中，从全局变量开始。 可以从 C++ 代码在外部修改这些全局变量。 
可以使用多种类型的变量，例如 int 或 float，然后在外部设置它们以供着色器程序使用。 
通常，将大多数全局变量放在称为“cbuffer”的缓冲区对象类型中，即使它只是一个全局变量。 
从逻辑上组织这些缓冲区对于着色器的高效执行以及显卡如何存储缓冲区非常重要。 

这里将三个矩阵放在同一个缓冲区中，因为将在每一帧中同时更新它们。*/
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//TYPEDEFS
//顶点输入类型
struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

//像素输入类型
struct PixelInputType
{
	float4 position:SV_POSITION;
    float4 color : COLOR;
};

//顶点着色器的处理函数，由GPU调用，会传入 顶点缓冲区中的数据格式 ，便于阅读，这个数据格式可以转为上面定义的 VertexInputType 类型
//然后这个处理函数会返回一个像素信息，这个信息会发送给像素着色器

//顶点着色器
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    //将位置矢量更改为4个单位，以便进行正确的矩阵计算。
    input.position.w = 1.0f;
    
    //依次乘以世界、视图和投影矩阵，用于计算顶点在屏幕上的渲染位置
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.color = input.color;
    //output.color.a *= 0.5f;
    
    return output;
}