//#include "stdafx.h"
#include "FBXLoader.h"
#include <set>

namespace FBXLoader
{

	////////////////////////////////////////////////////////////////////////////
	// Forward declaration of internal methods used by FBXLoader::Load method
	//
	// Notes: LoadCurve and GetKeyTimes are provided for you. LoadTexture has code for extracting
	// texture filename but you will need to add functionality for what to do with that file.
	// The rest of the methods will need to be filled in but have high level directions for 
	// you to use.

	////////////////////////////////////////////////////////////////////////////
	// LoadHierarchy:    Load the hierarchy from the FBX file.  The
	//                   hierarchy.GetNodes( ) and fbx_joints arrays are
	//                   parallell.
	// [in]scene:        The FBX scene.
	// [out]heirarchy:   The collection of transformNodes in our hierarchy.
	// [out]fbx_joints:  The FBX representation of the hierarchy.
	// return:           True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadHierarchy(FbxScene* scene, std::vector<TransformNode> &heirarchy,
		std::vector< FbxNode* >& fbx_joints);

	////////////////////////////////////////////////////////////////////////////
	// LoadHierarchy:    Link the hierarchy together.  This function will set
	//                   parent indices and children indices.
	// [out]heirarchy:   The collection of transformNodes in our hierarchy.
	// [in]fbx_joints:   The FBX representation of the hierarchy.
	// return:           True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LinkHierarchy(std::vector<TransformNode> &hierarchy,
		std::vector< FbxNode* >& fbx_joints);

	////////////////////////////////////////////////////////////////////////////
	// LoadAnimation:   Load an animation.
	// [in]anim_stack:  The animation stack to load the animation from.
	// [in]heirarchy:   The hierarchy.
	// [out]animation:  The loaded animation.
	// [in]fbx_joints:  The fbx_joints loaded from the scene.
	// Return:          True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadAnimation(FbxScene* scene, std::vector<TransformNode> &hierarchy,
		Animation &animation, std::vector< FbxNode* >& fbx_joints);

	////////////////////////////////////////////////////////////////////////////
	// LoadCurve:       Load the list of unique key times for a single curve.
	// [in]anim_curve:  The animation curve to use.
	// [out]key_times:  The list of unique key times for this animation curve.
	////////////////////////////////////////////////////////////////////////////
	static void LoadCurve(FbxAnimCurve* anim_curve,
		std::set< float >& key_times);

	////////////////////////////////////////////////////////////////////////////
	// KeyReduction:       Perform key reduction on an animation.
	// [in/out]animation:  The animation to perform key reduction on.
	////////////////////////////////////////////////////////////////////////////
	static void KeyReduction(Animation &animation);

	//
	// End Forward declaration of internal methods used by FBXLoader::Load method
	////////////////////////////////////////////////////////////////////////////

	bool Load(const std::string &fileName,
		// [out] A container of all the joint transforms found. As these will all be in the same 
		// hierarchy, you may only need the root instead of a list of all nodes.
		std::vector<TransformNode> &transformHierarchy,
		// [out] The loaded animation
		Animation &animation)
	{
		// Get an FBX manager
		FbxManager* manager = FbxManager::Create();
		if (manager == 0)
		{
			return false;
		}

		// Create IO settings
		FbxIOSettings* io_settings = FbxIOSettings::Create(manager, IOSROOT);
		if (io_settings == 0)
		{
			return false;
		}

		manager->SetIOSettings(io_settings);

		// Create importer
		FbxImporter* importer = FbxImporter::Create(manager, "");
		if (importer == 0)
		{
			return false;
		}

		// Initialize importer
		if (importer->Initialize(fileName.c_str(), -1, io_settings) == false)
		{
			std::string error;
			error += "FbxImporter::Initialize failed:  ";
			error += importer->GetStatus().GetErrorString();
#ifdef UNICODE
			std::wstring wideError = std::wstring(error.begin(), error.end());
			MessageBox(NULL, wideError.c_str(), L"Error", MB_OK);
#else
			MessageBox(NULL, error.c_str(), "Error", MB_OK);
#endif
			return false;
		}

		// Create a scene
		FbxScene* scene = FbxScene::Create(manager, "myScene");
		if (scene == 0)
		{
			return false;
		}

		// Load the scene with contents from the file
		if (importer->Import(scene) == false)
		{
			return false;
		}

		// No longer need the importer
		importer->Destroy();

		// Make sure everything is triangulated
		FbxGeometryConverter converter(manager);
		if (converter.Triangulate(scene, true) == false)
		{
			return false;
		}

		// If multiple materials are applied to a single mesh, split the mesh into separate meshes
		if (converter.SplitMeshesPerMaterial(scene, true) == false)
		{
			return false;
		}

		// Our list of unique joints
		std::vector< FbxNode* > fbx_joints;

		// Load hierarchy
		if (LoadHierarchy(scene, transformHierarchy, fbx_joints) == false)
		{
			return false;
		}

		// Link hierarchy
		if (LinkHierarchy(transformHierarchy, fbx_joints) == false)
		{
			return false;
		}
		if (LoadAnimation(scene, transformHierarchy, animation, fbx_joints) == false)
		{
			return false;
		}

		// Perform key reduction on the animation
		KeyReduction(animation);

		return true;
	}

	bool LoadHierarchy(FbxScene* scene, std::vector<TransformNode> &transformHierarchy,
		std::vector< FbxNode* >& fbx_joints)
	{
		/*
				TODO
				HierarchyNode objects name. The name can be obtained by calling
				FbxNodeAttribute::GetName. The following API functions are relevant to this function:
				•	FbxScene::GetSrcObjectCount
				•	FbxScene::GetSrcObject
				•	FbxNode::GetNodeAttributeCount
				•	FbxNode::GetNodeAttributeByIndex
				•	FbxNodeAttribute::GetAttributeType
				*/
		size_t ObjCount = scene->GetSrcObjectCount();
		for (size_t i = 0; i < ObjCount; i++)
		{
			FbxNode* Object = (FbxNode*)scene->GetSrcObject(i);
			size_t attrCount = Object->GetNodeAttributeCount();
			for (size_t i = 0; i < attrCount; i++)
			{
				if (Object->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
				{
					fbx_joints.push_back(Object);
					FbxSkeleton* Skely = (FbxSkeleton*)Object->GetNodeAttributeByIndex(i);
					TransformNode newNode;
					transformHierarchy.push_back(newNode);
				}
			}
		}
		return true;
	}

	bool LinkHierarchy(std::vector<TransformNode> &transformHierarchy,
		std::vector< FbxNode* >& fbx_joints)
	{
		for (size_t i = 0; i < fbx_joints.size(); i++)
		{
			FbxNode* node = fbx_joints[i];
			FbxNode* parent = node->GetParent();
			transformHierarchy[i].parentInd = MAXSIZE_T;
			for (size_t c = 0; c < fbx_joints.size(); ++c)
			{
				if (fbx_joints[c] == parent)
				{
					transformHierarchy[i].parentInd = c;
					break;
				}
			}
		}
		return true;
	}
	/*
		TODO
		The FBXLoader::LinkHierarchy function is responsible for setting the parent and
		children for the TransformNode objects in the hierarchy. You should loop through
		all of the fbx_joint elements, and set those indices appropriately.
		For each node, you should attempt to find its parent in the fbx_joints array. If the
		parent does not exist in the array, you can go to the next parent and attempt to find that.
		If you make it past the root of the tree, then your current node is a root node. Make
		sure to its parent to null to designate this.
		If you found a parent node, then set the parent of your current node to that
		node.  At this point, you can also add the current node as a child
		of the parent node. The following API function is of interest here :
		•	FbxNode::GetParent*/

	bool LoadAnimation(FbxScene* scene, std::vector<TransformNode> &hierarchy,
		Animation &animation, std::vector< FbxNode* >& fbx_joints)
	{
		FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
		FbxString animStackName = currAnimStack->GetName();
		FbxString mAnimationName = animStackName.Buffer();
		FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
		FbxLongLong duration = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
		animation.SetDuration(end.GetSecondDouble());
		animation.keyFrames = new KeyFrame[(int)duration];
		KeyFrame baseKey(0, hierarchy.size());
		baseKey.joints = &hierarchy[0];
		for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
		{
			FbxTime currTime;
			currTime.SetFrame(i, FbxTime::eFrames24);
			animation.keyFrames[i] = baseKey;
			for (size_t j = 0; j < fbx_joints.size(); j++)
			{
				FbxAMatrix local = fbx_joints[j]->EvaluateLocalTransform(currTime);
				XMFLOAT4X4 xmLocal;
				for (size_t row = 0; row < 4; row++)
				{
					for (size_t col = 0; col < 4; col++)
					{
						xmLocal.m[row][col] = local.Get(row, col);
					}
				}
				animation.keyFrames[i].joints[j].SetLocal(xmLocal);
			}
			animation.keyFrames[i].keyTime = currTime.GetSecondDouble();
			animation.SetNumFrames(duration);
			animation.keyFrames[i].LinkJoints();
		}
		baseKey.joints = nullptr;// = &hierarchy[0];
		return true;
		// TODO
		// The following directions assume you are using channels, which allows for each joint
		// to have a varying number of frames in the animation. This is typicially a strong optimization
		// as some joints will not move as much as others in a given animation. If using channels,
		// we will want to query when each key was made for each joint in the animation, then extract
		// the transform for that joint at the found time. 

		// The FBX SDK will give you allow you to extract transformations for a joint at anytime, 
		// even if the asset did not have a key made at the given time. If not using channels, a
		// keyframe of all joint transformations can be found key times that we provide at a fixed rate
		// such as 24, 30 0r 60 frames per second. For additional support extracting animation without
		// using channels, refer to this external source tutorial: 
		// http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582
		// Otherwise...

		// The FBXLoader::LoadAnimation function is responsible for loading the actual animation 
		// data. You will loop through each of the fbx_joint elements, obtaining a single channel 
		// of the animation for each. You can call FBXLoader::GetKeyTimes to obtain all the unique 
		// key times that an FbxNode was keyed at. You will need to loop through all of those key 
		// times, excluding the first, and store a key (transform and time) 
		// in the current channel. The collection of keys will produce the channel of animation. The 
		// first key time is ignored because the first key represents the bind pose in the model 
		// you will be importing. After the animation channels are loaded, you may want to do a 
		// second pass to "sanitize" the data. If any channel has 0 keys, you can add a key with 
		// the bind pose transform. The key time should be set to 0.0f. The following are API calls 
		// of interest :
		//	•	FbxTime::SetSecondDouble
		//	•	FbxNode::EvaluateGlobalTransform
	}

	void GetKeyTimes(FbxAnimStack* anim_stack, FbxNode* node, std::set< float >& key_times)
	{
		FbxAnimCurve* anim_curve;

		// Get the number of animation layers in the current animation stack
		int num_anim_layers = anim_stack->GetMemberCount< FbxAnimLayer >();

		FbxAnimLayer* anim_layer;
		for (int j = 0; j < num_anim_layers; ++j)
		{
			anim_layer = anim_stack->GetMember< FbxAnimLayer >(j);

			anim_curve = node->LclTranslation.GetCurve(anim_layer);
			if (anim_curve != 0)
			{
				LoadCurve(anim_curve, key_times);
			}

			anim_curve = node->LclRotation.GetCurve(anim_layer);
			if (anim_curve != 0)
			{
				LoadCurve(anim_curve, key_times);
			}

			anim_curve = node->LclScaling.GetCurve(anim_layer);
			if (anim_curve != 0)
			{
				LoadCurve(anim_curve, key_times);
			}
		}
	}

	void LoadCurve(FbxAnimCurve* anim_curve, std::set< float >& key_times)
	{
		int key_count = anim_curve->KeyGetCount();
		for (int key = 0; key < key_count; ++key)
		{
			float key_time = (float)anim_curve->KeyGetTime(key).GetSecondDouble();
			key_times.insert(key_time);
		}
	}

	void KeyReduction(Animation& animation)
	{
		// TODO ( optional )
		// We could use this method tio attempt to detect if any keyframes/channel keys are not 
		// needed. We can do this by calculating the difference between neighboring keys 
		// (previous & next) and if this difference is less than some minimum threshold remove one
		// of the keys. We would typically remove the "next" key of the neighboring pair, but there 
		// may be special cases.

		// If you are not using channels, then the difference must be less than the threshold for all 
		// joints at that keyframe. This may seem unlikely to happen, but if you extracted keys from 
		// FBX at a fixed interval, as the external web source provided suggests, then you likely will 
		// have more keys then needed.

		// If you are using channels and extracting keys at the keytimes stored in the asset, which 
		// would have been created by an artist, we only need to check neighboring keys in the given 
		// channel, and do this for each channel. That being said, you are less likely to find keys that
		// can safetly be removed as the artist making the asset may have already done that for you.
		// Given time trying this out can still be useful and informative. You might attempt to see how 
		// high you can set that minimum threshold for a particular asset before lost keys cause the 
		// animation to not look correct. FYI, for rotations with quaternions, keys can typically be
		// properly interpolated as long as they are under 180 degrees different.
	}
};
