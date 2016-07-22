#pragma once
#include "Animation.h"

class Interpolator
{
public:
	Interpolator();
	Interpolator(Animation *anim) : animPtr(anim){ next_frame_index = 0; currentTime = 0.00f; }
	~Interpolator();

	inline void AddTime(float timeToAdd) { currentTime += timeToAdd; }
	void SetTime(float _currentTime);// { currentTime = _currentTime; }
	inline void SetAnimPtr(Animation* anim){ animPtr = anim; }

	// Creates the "betweenKeyFrame" data based on the animation and currentTime elements
	void Process();
	size_t next_frame_index;
	float currentTime;

	// The animation this interpolator is interpolating
	Animation *animPtr;

	// The result of the interpolation, if not using channels
	KeyFrame betweenKeyFrame;

	// Or if using channels, we would have one result per joint
	//KeyFrame betweenKeyFrameChannel[NUMBER_OF_JOINTS_IN_RIG]
};

