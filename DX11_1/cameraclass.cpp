#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0;
	m_positionY = 0;
	m_positionZ = 0;

	m_rotationX = 0;
	m_rotationY = 0;
	m_rotationZ = 0;
}

CameraClass::CameraClass(const CameraClass&)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX,m_positionY,m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX,m_rotationY,m_rotationZ);
}

/*Render 函数使用相机的位置和旋转来构建和更新视图矩阵。 
首先设置向上、位置、旋转等变量。 然后在场景的原点，首先根据摄像机的 x、y 和 z 旋转来旋转摄像机。 
一旦它正确旋转，然后将相机平移到 3D 空间中的位置。 
有了正确的值，就可以使用 XMMatrixLookAtLH 函数来 创建视图矩阵以表示当前摄像机的旋转和平移。*/

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//设置朝上的向量
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	//将其加载到XMVECTOR结构中
	upVector = XMLoadFloat3(&up);

	//设置相机在世界中的位置
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	//载入到结构体
	positionVector = XMLoadFloat3(&position);

	//设置相机默认朝向
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	//以弧度设置旋转轴相关值
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;
	//创建旋转矩阵
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//通过旋转矩阵变换lookAt和向上矢量，使视图在原点处正确旋转。
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//将旋转的摄影机位置平移到查看器的位置。
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//最后，根据三个更新的矢量创建视图矩阵。
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

//视图矩阵将是 HLSL 顶点着色器中使用的三个主要矩阵之一

//获取视图矩阵
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
