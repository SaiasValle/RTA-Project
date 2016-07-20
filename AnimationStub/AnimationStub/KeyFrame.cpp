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
	LinkJoints();
}

KeyFrame& KeyFrame::operator= (const KeyFrame& copyMe)
{
	keyTime = copyMe.keyTime; 
	num_bones = copyMe.num_bones;
	joints = new TransformNode[num_bones];
	memcpy(joints, copyMe.joints, sizeof(TransformNode)*num_bones);
	return *this;
}

void KeyFrame::LinkJoints()
{
	for (size_t i = 0; i < num_bones; i++)
	{
		joints[i].children.clear();
	}
	for (size_t i = 0; i < num_bones; i++)
	{
		if (joints[i].parentInd != MAXSIZE_T){
			joints[i].parentPtr = &joints[joints[i].parentInd];
			joints[joints[i].parentInd].children.push_back(&joints[i]);
		}
		else
			joints[i].parentPtr = nullptr;
	}
}

KeyFrame::~KeyFrame()
{
	delete[] joints;
}
