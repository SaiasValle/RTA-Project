#include "stdafx.h"
#include "KeyFrame.h"


KeyFrame::KeyFrame() : keyTime(0), num_bones(0)
{
	joints = nullptr;
}

KeyFrame::KeyFrame(float Time, int numBones) : keyTime(Time), num_bones(numBones)
{
	joints = nullptr;
	joints = new TransformNode[numBones];
}

KeyFrame::KeyFrame(const KeyFrame& copyMe) : keyTime(copyMe.keyTime), num_bones(copyMe.num_bones)
{
	joints = new TransformNode[num_bones];
	memcpy(joints, copyMe.joints, sizeof(TransformNode)*num_bones);
}

KeyFrame::~KeyFrame()
{
	delete[] joints;
}
