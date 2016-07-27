#include "stdafx.h"
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
	// LoadMesh:                    Load a mesh from an FbxMesh attribute.
	// [in]fbx_mesh:                The FbxMesh attribute to load data from.
	// [out]mesh:                   The mesh data loaded from the FbxMesh
	//                              attribute.
	// [in]fbx_joints:              The FBX representation of the hierarchy.
	// [out]control_point_indices:  The control point indices from which the
	//                              unique vertex originated.  This array and
	//                              the unique vertex array in the mesh would
	//                              be parallell containers.
	// return:                      True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadMesh(FbxMesh* fbx_mesh, Mesh& mesh,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices);

	//////////////////////////////////////////////////////////////////////////////
	//// GetBindPose:  Get the bind pose transform of a joint.
	//// [in]cluster   The FBX cluster.
	//// return:       The bind pose matrix.
	//////////////////////////////////////////////////////////////////////////////
	FbxAMatrix GetBindPose(FbxNode* mesh_node, FbxCluster* cluster);

	////////////////////////////////////////////////////////////////////////////
	// LoadSkin:                   Load skin data from an FbxMesh attribute.
	// [in]fbx_mesh:               The FbxMesh attribute to load data from.
	// [out]mesh:                  The mesh data loaded from the FbxMesh
	//                             attribute.
	// [out]heirarchy:             The collection of transformNodes in our hierarchy.
	// [out]fbx_joints:            The FBX joints loaded from the scene.
	// [in]control_point_indices:  The control point indices from which the
	//                             unique vertex originated.  This array and
	//                             the unique vertex array in the mesh are
	//                             parallell arrays.
	// return:                     True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadSkin(FbxMesh* fbx_mesh, Mesh& mesh, std::vector<TransformNode> &hierarchy,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices);


	////////////////////////////////////////////////////////////////////////////
	// LoadTexture:   Load the texture name from a mesh.
	// [in]fbx_mesh:  The FbxMesh attribute to load data from.
	// [out]mesh:     The mesh data loaded from the FbxMesh attribute.
	// return:        True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadTexture(FbxMesh* fbx_mesh, Mesh& mesh);

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
		// [out] Test nmodels provided will only have one mesh, but other assets may have multiple
		// meshes using the same rig to create a model
		std::vector<Mesh > &meshes,
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

		// Load Mesh data
		for (int i = 0; i < scene->GetSrcObjectCount< FbxMesh >(); ++i)
		{
			FbxMesh* mesh_attribute = scene->GetSrcObject< FbxMesh >(i);

			Mesh mesh;
			std::vector< unsigned int > control_point_indices;

			if (LoadMesh(mesh_attribute, mesh, fbx_joints, control_point_indices) == false)
			{
				return false;
			}
			if (LoadSkin(mesh_attribute, mesh, transformHierarchy, fbx_joints,
				control_point_indices) == false)
			{
				return false;
			}
			if (LoadTexture(mesh_attribute, mesh) == false)
			{
				return false;
			}

			meshes.push_back(mesh);
		}

		// Get the number of animation stack


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

	bool LoadMesh(FbxMesh* fbx_mesh, Mesh& mesh,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices)
	{
		FbxVector4* controlPoints = fbx_mesh->GetControlPoints();
		size_t numCP = fbx_mesh->GetControlPointsCount();
		for (size_t j = 0; j < fbx_mesh->GetPolygonCount(); ++j)
		{
			for (size_t k = 0; k < 3; ++k)
			{
				int index = fbx_mesh->GetPolygonVertex(j, k);
				Vertex temp;
				temp.pos[0] = (float)controlPoints[index].mData[0];
				temp.pos[1] = (float)controlPoints[index].mData[1];
				temp.pos[2] = (float)controlPoints[index].mData[2];
				temp.pos[3] = 1;
				FbxGeometryElementUV* UV = fbx_mesh->GetElementUV();
				temp.uv[0] = (float)UV->GetDirectArray().GetAt(index).mData[0];
				temp.uv[1] = (float)UV->GetDirectArray().GetAt(index).mData[1];
				temp.Bones[0] = -1; temp.Bones[1] = -1;
				temp.Bones[2] = -1; temp.Bones[3] = -1;
				size_t c = 0;
				bool found = false;
				for (; c < mesh.verts.size(); c++)
				{
					if (mesh.verts[c].pos == temp.pos)
						if (mesh.verts[c].uv == temp.uv)
						{
							found = true;
							break;
						}
				}
				control_point_indices.push_back(index);
				if (found)
				{
					mesh.index.push_back(c);
					continue;
				}
				else
				{
					mesh.verts.push_back(temp);
					mesh.index.push_back(mesh.verts.size() - 1);
				}
			}
		}
		return true;
	}
	// TODO
	// Get control points - fbx_mesh->GetControlPoints()
	// For each polygon in mesh
	// For each vertex in current polygon
	// Get control point index - fbx_mesh->GetPolygonVertex(...)
	// Get Position of vertex
	// Get Texture Coordinates
	// Get Normals
	// Get any other needed mesh data, such as tangents
	// Iterate through unique vertices found so far...
	// if this vertex is unique add to set of unique vertices
	// Push index of where vertex lives in unique vertices container into index 
	// array, assuming you are using index arrays which you generally should be
	// End For each vertex in current polygon
	// End For each polygon in mesh
#define F_EPSILON 0.001
	bool LoadSkin(FbxMesh* fbx_mesh, Mesh& mesh, std::vector<TransformNode> &hierarchy,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices)
	{
		FbxSkin* skin = (FbxSkin*)fbx_mesh->GetDeformer(0);
		size_t clustercount = skin->GetClusterCount();
		FbxAMatrix* bindarray = new FbxAMatrix[clustercount];
		for (size_t i = 0; i < clustercount; i++)
		{
			FbxCluster* cluster = skin->GetCluster(i);
			FbxNode* bone = cluster->GetLink();
			bindarray[i] = GetBindPose(bone, cluster);
			size_t boneIndex;
			for (boneIndex = 0; boneIndex < fbx_joints.size(); ++boneIndex)
			{
				if (fbx_joints[boneIndex] == bone)
					break;
			}
			size_t influenceCount = cluster->GetControlPointIndicesCount();
			int* influence = cluster->GetControlPointIndices();
			double* influenceWeight = cluster->GetControlPointWeights();
			for (size_t j = 0; j < influenceCount; j++)
			{
				if (influenceWeight[j] <= F_EPSILON)
					continue;
				size_t k = 0;
				for (; k < control_point_indices.size(); k++)
				{
					if (influence[j] == control_point_indices[k]){
						k = mesh.index[k];
						break;
					}
				}
				bool set = false;
				int ind = 0; float min = FBXSDK_FLOAT_MAX;
				for (size_t c = 0; c < 4; c++)
				{
					if (mesh.verts[k].Bones[c] == -1)
					{
						mesh.verts[k].Bones[c] = boneIndex;
						mesh.verts[k].Weights[c] = influenceWeight[j];
						set = true;
						break;
					}
					else if (mesh.verts[k].Weights[c] < min)
					{
						mesh.verts[k].Weights[c];
						ind = c;
					}
				}
				if (!set && influenceWeight[j] > mesh.verts[k].Weights[ind])
				{
					mesh.verts[k].Bones[ind] = boneIndex;
					mesh.verts[k].Weights[ind] = influenceWeight[j];
				}
			}
		}
		for (size_t i = 0; i < mesh.verts.size(); i++)
		{
			FbxAMatrix result; result.SetIdentity();
			for (size_t j = 0; j < 4; ++j)
			{
				int index = mesh.verts[i].Bones[j];
				FbxAMatrix temp = bindarray[index] * (double)mesh.verts[i].Weights[j];
				for (size_t c = 0; c < 4; c++)
				{
					for (size_t x = 0; x < 4; x++)
					{
						result.mData[c][x] += temp.mData[c][x];
					}
				}
			}
			result = result.Inverse();
			FbxVector4 position = FbxVector4(mesh.verts[i].pos[0], mesh.verts[i].pos[1], mesh.verts[i].pos[2], mesh.verts[i].pos[3]);
			position = result.MultT(position);
			for (size_t k = 0; k < 4; k++)
			{
				mesh.verts[i].pos[k] = position[k];
			}
		}
		delete[] bindarray;
		return true;
	}
	/*
	TODO
	The FBXLoader::LoadSkin function is the function used to load skinning information.
	This function will primarily be tasked with extracting joint influence data,
	weight and index, for each vertex.

	You will loop through the number of skin deformers in the FbxMesh. For each skin
	deformer, you will loop through the skin clusters. Each skin cluster represents one
	joint's effect on a cluster of control points. The cluster's link represents the joint.
	You will have to find the index of the joint by searching through the fbx_joints
	vector. Once you know the index, you will have to calculate the world bind pose
	transform of the joint. You can call FBXLoader::GetBindPose to obtain this matrix and
	store the transform in the TransformNode at that index in 'hierarchy'.

	You will now need to load influence data from the cluster. Each control point index
	in the cluster will be used to find an influence for the current joint. You should
	reject influences if their weight is negligible(something less than 0.001f).
	This will avoid unnecessary processing. The following API methods are or interest for
	the implementation of this function:
	•	FbxMesh::GetDeformerCount
	•	FbxMesh::GetDeformer
	•	FbxSkin::GetClusterCount
	•	FbxSkin::GetCluster
	•	FbxCluster::GetLink
	•	FbxCluster::GetControlPointIndicesCount
	•	FbxCluster::GetControlPointIndices
	•	FbxCluster::GetControlPointWeights
	*/

	FbxAMatrix GetBindPose(FbxNode* mesh_node, FbxCluster* cluster)
	{
		FbxVector4 translation = mesh_node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 rotation = mesh_node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 scaling = mesh_node->GetGeometricScaling(FbxNode::eSourcePivot);
		FbxAMatrix mesh_geometry_transform = FbxAMatrix(translation, rotation, scaling);

		FbxAMatrix bind_pose_transform;
		cluster->GetTransformLinkMatrix(bind_pose_transform);

		FbxAMatrix cluster_transform;
		cluster->GetTransformMatrix(cluster_transform);

		FbxAMatrix result = mesh_geometry_transform * cluster_transform.Inverse() * bind_pose_transform;

		return result;
	}

	bool LoadTexture(FbxMesh* fbx_mesh, Mesh& mesh)
	{
		FbxProperty property;

		if (fbx_mesh->GetNode() == NULL)
			return false;

		int material_count = fbx_mesh->GetNode()->GetSrcObjectCount< FbxSurfaceMaterial >();

		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetSrcObject< FbxSurfaceMaterial >(material_index);

			if (surface_material == 0)
				continue;

			int texture_index;

			FBXSDK_FOR_EACH_TEXTURE(texture_index)
			{
				property = surface_material->FindProperty(FbxLayerElement::sTextureChannelNames[texture_index]);

				if (property.IsValid() == false)
					continue;

				int texture_count = property.GetSrcObjectCount< FbxTexture >();

				for (int i = 0; i < texture_count; ++i)
				{
					// Ignore layered textures

					FbxTexture* texture = property.GetSrcObject< FbxTexture >(i);
					if (texture == 0)
						continue;

					FbxFileTexture* file_texture = FbxCast< FbxFileTexture >(texture);
					if (file_texture == 0)
						continue;

					std::string texture_name = file_texture->GetFileName();

					// TODO : something with the texture name here....

					/*std::string::size_type pos = texture_name.find_last_of("/\\");
					if (pos != std::string::npos)
					{
					texture_name = texture_name.substr(pos + 1);
					}*/
				}
			}
		}

		return true;
	}

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
