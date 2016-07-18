#include "stdafx.h"
#include "Animation.h"


Animation::Animation() : animName("I forgot to set the animName.")
{
	num_frames = 0;
	keyFrames = nullptr;
}


Animation::~Animation()
{
	delete[] keyFrames;
}
