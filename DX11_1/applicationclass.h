#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

//#include <windows.h>
#include "d3dclass.h"
#include "cameraclass.h"
#include "ModelClass.h"
//#include "colorshaderclass.h"
#include "textureshaderclass.h"

//�Ƿ�ȫ��
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	//ColorShaderClass* m_ColorShader;

	TextureShaderClass* m_TextureShader;
};

#endif
