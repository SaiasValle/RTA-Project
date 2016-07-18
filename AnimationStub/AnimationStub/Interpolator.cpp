#include "stdafx.h"
#include "Interpolator.h"

#include <assert.h>

Interpolator::Interpolator() : animPtr(nullptr)
{
	betweenKeyFrame = nullptr;
}


Interpolator::~Interpolator()
{
	delete betweenKeyFrame;
}

void Interpolator::SetTime(float _currentTime)
{
	currentTime = _currentTime;
	if (animPtr){
		next_frame_index = 0;
		float keyTime = animPtr->GetFramesPtr()[next_frame_index].keyTime;
		while (keyTime < _currentTime)
		{
			++next_frame_index;
			keyTime = animPtr->GetFramesPtr()[next_frame_index].keyTime;
		}
	}
}

void Interpolator::Process()
{
	// You set the animation pointer, right?
	assert(animPtr);
	size_t& next = next_frame_index;

	// Make sure currentTime is valid, assuming we want to loop animations
	float animDuration = animPtr->GetDuration();
	while (currentTime > animDuration){
		currentTime -= animDuration;
		next = 1;
	}
	// TODO: Interpolate you keyframe or channel data here

	//Find the two Frames we are to interpolate between
	KeyFrame* frameList = animPtr->GetFramesPtr();
	if (frameList == nullptr)
		return;
	while (frameList[next].keyTime <= currentTime && next != animPtr->GetNumFrames())
		++next;
	size_t prev = next - 1;
	KeyFrame* PrevFrame, *NextFrame;
	PrevFrame = &frameList[prev];
	NextFrame = &frameList[next];
	delete betweenKeyFrame;
	betweenKeyFrame = new KeyFrame(*PrevFrame);
	//Begin interpolation
	for (size_t i = 0; i < PrevFrame->num_bones; i++)
	{
		const XMFLOAT4X4* prevMatrix = &PrevFrame->joints[i].GetLocal();
		const XMFLOAT4X4* nextMatrix = &NextFrame->joints[i].GetLocal();
		//Slerp of Rotation Data
		XMVECTOR Quat1 = XMQuaternionRotationMatrix(XMLoadFloat4x4(prevMatrix));
		XMVECTOR Quat2 = XMQuaternionRotationMatrix(XMLoadFloat4x4(nextMatrix));
		float t = currentTime / animDuration;
		Quat1 = XMQuaternionSlerp(Quat1, Quat2, t);
		XMFLOAT4X4 Rotation;
		XMStoreFloat4x4(&Rotation, XMMatrixRotationQuaternion(Quat1));
		//Linear interpolation of Position Data			// = (1-t)*frame[n] + t*frame[n-1]
		XMFLOAT3 trans;
		trans.x = (1 - t)*(*prevMatrix).m[3][0] + t*(*nextMatrix).m[3][0];
		trans.y = (1 - t)*(*prevMatrix).m[3][1] + t*(*nextMatrix).m[3][1];
		trans.z = (1 - t)*(*prevMatrix).m[3][2] + t*(*nextMatrix).m[3][2];
		Rotation.m[3][0] = trans.x;
		Rotation.m[3][1] = trans.y;
		Rotation.m[3][2] = trans.z;
		betweenKeyFrame->joints[i].SetLocal(Rotation);
	}
}