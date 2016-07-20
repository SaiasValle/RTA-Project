#pragma once
#include <string>

#include "KeyFrame.h"

struct Channel
{
	// This should not be a constant in your version
	static const int NUMBER_OF_KEYS_FOR_THIS_JOINT = 42;

	KeyFrame keyFrames[NUMBER_OF_KEYS_FOR_THIS_JOINT];
};

class Animation
{
public:
	Animation();
	~Animation();

	inline float GetDuration() { return duration; }
	inline void SetDuration(float _duration) { duration = _duration; }

	inline const std::string &GetName() const { return animName; }
	inline void SetName(const std::string &name) { animName = name; }
	inline KeyFrame* GetFramesPtr(){ return keyFrames; }
	inline size_t GetNumFrames(){ return num_frames; }
	KeyFrame* keyFrames;
private:
	std::string animName;
	
	// The total length of the animation
	float duration;
	size_t num_frames;
	// The representation of the animation at a certain point in time

	// Or if using channels, remove the array of keyFrames above and instead do something like:


	//Channel channels[NUMBER_OF_JOINTS_IN_RIG];

};

