#pragma once
#include "GraphicsManager.h"

#define MOUSE_ROT_MODIFIER 0.0003f
#define MOUSE_MOVE_MODIFIER 0.04f

class Camera
{
private:
	XMFLOAT4X4 m_CameraMat;
	POINT m_PrevPos;

public:
	Camera();
	~Camera(void);

	XMFLOAT4X4 GetCameraMat() { return m_CameraMat; }
	void SetCameraMat(XMFLOAT4X4 camMatrix) { m_CameraMat = camMatrix; }

	void CameraControl(XMFLOAT4X4 &viewMat);
};

