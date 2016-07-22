//#include "stdafx.h"
#include "TransformNode.h"


TransformNode::TransformNode() : parentPtr(nullptr)
{
	name = "Oh crap, I forgot to set the name";
}


TransformNode::~TransformNode()
{
}

const Transform& TransformNode::GetWorld()
{
	// TODO
	// add code here... 
	if (bDirty){
		if (!parentPtr)
		{
			world = local;
		}
		else
		{
			XMMATRIX Local, Parent;
			Local = XMLoadFloat4x4(&local);
			Parent = XMLoadFloat4x4(&parentPtr->GetWorld());
			XMStoreFloat4x4(&world, XMMatrixMultiply(Local, Parent));
		}
		bDirty = false;
	}
	return world;
}

void TransformNode::SetLocal(const Transform &_local)
{
	local = _local;
	MakeDirty();
}

void TransformNode::MakeDirty()
{
	bDirty = true;
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->MakeDirty();
	}
	// Your code here...or better yet, in a source file
}
