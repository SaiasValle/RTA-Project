#include "Camera.h"


Camera::Camera()
{
	XMStoreFloat4x4(&m_CameraMat, XMMatrixIdentity());
}


Camera::~Camera()
{
}

void Camera::CameraControl(XMFLOAT4X4 &viewMat)
{
	// Store Inverse
	XMStoreFloat4x4(&m_CameraMat, XMMatrixInverse(0, XMLoadFloat4x4(&viewMat)));

	// Translation
#if 1
	if (GetAsyncKeyState('E'))
	{
		XMVECTOR move{ 0, MOUSE_MOVE_MODIFIER, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('Q'))
	{
		XMVECTOR move = { 0, -MOUSE_MOVE_MODIFIER, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('W'))
	{
		XMVECTOR move = { 0, 0, MOUSE_MOVE_MODIFIER, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('S'))
	{
		XMVECTOR move = { 0, 0, -MOUSE_MOVE_MODIFIER, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('D'))
	{
		XMVECTOR move = { MOUSE_MOVE_MODIFIER, 0, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('A'))
	{
		XMVECTOR move = { -MOUSE_MOVE_MODIFIER, 0, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	// Reset Camera to Origin
	if (GetAsyncKeyState(VK_BACK))
	{
		XMStoreFloat4x4(&m_CameraMat, XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	}
#endif

	// Rotation
#if 1
	POINT currPos;
	GetCursorPos(&currPos);

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		float mouseDiff[2] = { float(currPos.x - m_PrevPos.x), float(currPos.y - m_PrevPos.y) };

		XMMATRIX camera_mat = XMLoadFloat4x4(&m_CameraMat);
		XMVECTOR pos;
		pos.m128_f32[0] = camera_mat.r[3].m128_f32[0];
		pos.m128_f32[1] = camera_mat.r[3].m128_f32[1];
		pos.m128_f32[2] = camera_mat.r[3].m128_f32[2];

		camera_mat.r[3] = XMVectorSet(0, 0, 0, 1);

		// Up & Down
		XMMATRIX rotate = XMMatrixRotationX(mouseDiff[1] * MOUSE_ROT_MODIFIER);
		camera_mat = XMMatrixMultiply(rotate, camera_mat);

		// Right & Left
		rotate = XMMatrixRotationY(mouseDiff[0] * MOUSE_ROT_MODIFIER);
		camera_mat = XMMatrixMultiply(camera_mat, rotate);

		camera_mat.r[3].m128_f32[0] = pos.m128_f32[0];
		camera_mat.r[3].m128_f32[1] = pos.m128_f32[1];
		camera_mat.r[3].m128_f32[2] = pos.m128_f32[2];

		XMStoreFloat4x4(&m_CameraMat, camera_mat);
	}

	m_PrevPos = currPos;
#endif
	// Return View Matrix
	XMStoreFloat4x4(&viewMat, XMMatrixInverse(0, XMLoadFloat4x4(&m_CameraMat)));
}