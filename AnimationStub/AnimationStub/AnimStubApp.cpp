#include "stdafx.h"
#include "AnimStubApp.h"
#include "FBXLoader.h"
#include "Interpolator.h"

AnimStubApp::AnimStubApp()
{
}


AnimStubApp::~AnimStubApp()
{
}

void AnimStubApp::Initialize()
{
	std::vector<Mesh > meshes;
	std::vector<TransformNode> transformHierarchy;
	Animation animation;
	FBXLoader::Load("Teddy_Idle.fbx", meshes, transformHierarchy, animation);

	Interpolator test(&animation);
	while (true){
		test.Process();
		test.AddTime(0.01f);
	}
}

void AnimStubApp::Update()
{

}

void AnimStubApp::Render()
{

}
