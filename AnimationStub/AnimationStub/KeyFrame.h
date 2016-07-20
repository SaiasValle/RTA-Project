#pragma once
#include "TransformNode.h"
class KeyFrame
{
public:
	KeyFrame();
	KeyFrame(float Time, int numBones);
	KeyFrame(const KeyFrame& copyMe);
	KeyFrame& operator= (const KeyFrame& copyMe);
	~KeyFrame();

public:
	// When in the animation does this KeyFrame happen?
	float keyTime;
	unsigned int num_bones;
	// The representation of the animated "skeleton" (All joints) at the keyTime
	void LinkJoints();
	TransformNode* joints;
	// Or.....

	// If using "channels", which is better, each joint would have a channel object
	// which collects the keyframes for that specific joint, allowing each joint to have 
	// a differing number of keyframes, but also requireing each joint to be interpolated
	// instead of the whole animation.

	// Refer to the "Common methods of computer animation" presentation for more information
	// on TransformNode, channels and additional animation topics.
};

