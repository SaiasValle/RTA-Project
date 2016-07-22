#pragma once
#include <string>
#include <vector>
#include <fbxsdk.h>

#include "TransformNode.h"
#include "Animation.h"

namespace FBXLoader
{
	// Loads an FBX file and returns a filled out mesh, transform hierarchy and animation
	bool Load(const std::string &fileName,
		// [out] A container of all the joint transforms found. As these will all be in the same 
		// hierarchy, you may only need the root instead of a list of all nodes.
		std::vector<TransformNode> &transformHierarchy,
		// [out] The loaded animation
		Animation &animation);
};

