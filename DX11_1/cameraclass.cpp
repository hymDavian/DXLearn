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

/*Render ����ʹ�������λ�ú���ת�������͸�����ͼ���� 
�����������ϡ�λ�á���ת�ȱ����� Ȼ���ڳ�����ԭ�㣬���ȸ���������� x��y �� z ��ת����ת������� 
һ������ȷ��ת��Ȼ�����ƽ�Ƶ� 3D �ռ��е�λ�á� 
������ȷ��ֵ���Ϳ���ʹ�� XMMatrixLookAtLH ������ ������ͼ�����Ա�ʾ��ǰ���������ת��ƽ�ơ�*/

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//���ó��ϵ�����
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	//������ص�XMVECTOR�ṹ��
	upVector = XMLoadFloat3(&up);

	//��������������е�λ��
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	//���뵽�ṹ��
	positionVector = XMLoadFloat3(&position);

	//�������Ĭ�ϳ���
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	//�Ի���������ת�����ֵ
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;
	//������ת����
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//ͨ����ת����任lookAt������ʸ����ʹ��ͼ��ԭ�㴦��ȷ��ת��
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//����ת����Ӱ��λ��ƽ�Ƶ��鿴����λ�á�
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//��󣬸����������µ�ʸ��������ͼ����
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

//��ͼ������ HLSL ������ɫ����ʹ�õ�������Ҫ����֮һ

//��ȡ��ͼ����
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
