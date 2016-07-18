#pragma once
#include <vector>
#include <string>
using std::vector;
using std::string;

// Defines a node instance for a transformation in a hierarchy
// There are a variety of means by which we could define a transorm.
// The simplest, but not usually most optimized transformation would just be a 
// 4x4 matrix which can represent translation, rotation and scale, but will have
// some amount of errors when interpolating rotations and is likely more "degrees of freedom"
// (DOF) than are being used.
// A common alternative is to use a vector for translation, a vector for scale
// and a quaternion for rotations. Quaternions can be interpolated without the errors
// of the matrix based rotation. We might ewven drop scale if our system is not using that
// particular set of DOFs (Scale is 3 DOFs, controlling scale in the X, Y & Z dimensions).
// Even though a majority of huminoid animations only rotate joints, since we are in a 
// hierarchy, if a parent node rotates, 
// its child will translate so we must keep translation typically.

// Most of the implementation for this object is defined in the "Common methods of computer 
// animation" presentation in the course materials folder.

#include "DirectXMath.h"
using namespace DirectX;

#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>

// Easier to Re-use the define than replace every instance.
#define Transform XMFLOAT4X4
class TransformNode
{
public:
	TransformNode();
	~TransformNode();

	inline const Transform &GetLocal()	{ return local;	}
	inline const string &GetName() { return name; }
	inline void SetName(const string &_name) { name = _name; }

	const Transform& GetWorld();

	void SetLocal(const Transform &_local);

private:
	// The world space transformation for this object. 
	Transform world;

	// The local space transformation for this object
	Transform local;

	// This nodes parent, null if this node is the root
	TransformNode *parentPtr;

	// This nodes collection of children. Leaf nodes will not have children.
	vector<TransformNode *>children;

	// If true, the world transform will need to be recalculated, 
    //else world is currently correct
	bool bDirty;

	void MakeDirty();

	// The name of this transform node, typically a joint name read from file
	//possibly replace with a uint ID easier to compare.
	string name;
};

