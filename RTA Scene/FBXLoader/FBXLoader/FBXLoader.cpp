// FBXLoader.cpp : Defines the entry point for the console application.
//

#include <fbxsdk.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>
using namespace DirectX;
using namespace std;

struct vert
{
	float pos[3];
	float uv[2];
	float norm[3];
	float tan[3];
};

int main(int argc, char* argv[])
{
	FbxManager* BK = FbxManager::Create();

	FbxIOSettings * settings = FbxIOSettings::Create(BK, IOSROOT);

	BK->SetIOSettings(settings);

	FbxImporter * import = FbxImporter::Create(BK, "");
	FbxScene * scene = FbxScene::Create(BK, "");
	string filename;
	if (argv[1]!= NULL)
	{
		filename = argv[1];
	}
	else
	{
		filename = "Teddy_Idle.fbx";
	}



	vector<vert>out;
	int numVerts;
	vector<int> indies;
	bool success = import->Initialize(filename.c_str(), -1, settings);
	if (!success)
		return EXIT_FAILURE;
	success = import->Import(scene);
	if (!success)
		return EXIT_FAILURE;

	import->Destroy();

	FbxNode* root = scene->GetRootNode();

	if (root)
	{
		size_t numchild = root->GetChildCount();
		for (int i = 0; i < numchild; i++)
		{
			FbxNode* Child = root->GetChild(i);
			if (Child->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType attribute = Child->GetNodeAttribute()->GetAttributeType();
			if (attribute != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* model = (FbxMesh*)Child->GetNodeAttribute();

			FbxVector4* verts = model->GetControlPoints();
			numVerts = model->GetControlPointsCount();

			for (int j = 0; j < model->GetPolygonCount(); j++)
			{
				int vertcount = model->GetPolygonSize(j);

				for (size_t k = 0; k < 3; k++)
				{
					int index = model->GetPolygonVertex(j, k);

					vert temp;

					temp.pos[0] = (float)verts[index].mData[0];
					temp.pos[1] = (float)verts[index].mData[1];
					temp.pos[2] = (float)verts[index].mData[2];

					FbxGeometryElementNormal* normal = model->GetElementNormal();


					switch (normal->GetMappingMode())
					{
					case FbxGeometryElement::eByPolygonVertex:
					{
						switch (normal->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						{
							temp.norm[0] = (float)normal->GetDirectArray().GetAt(index).mData[0];
							temp.norm[1] = (float)normal->GetDirectArray().GetAt(index).mData[1];
							temp.norm[2] = (float)normal->GetDirectArray().GetAt(index).mData[2];
							break;
						}
						case FbxGeometryElement::eIndexToDirect:
						{
							int index2 = normal->GetIndexArray().GetAt(index);
							temp.norm[0] = (float)normal->GetDirectArray().GetAt(index2).mData[0];
							temp.norm[1] = (float)normal->GetDirectArray().GetAt(index2).mData[1];
							temp.norm[2] = (float)normal->GetDirectArray().GetAt(index2).mData[2];
						}
						default:
							break;
						}
					}
					default:
						break;
					}

					FbxGeometryElementUV* UV = model->GetElementUV();

					temp.uv[0] = (float)UV->GetDirectArray().GetAt(index).mData[0];
					temp.uv[1] = (float)UV->GetDirectArray().GetAt(index).mData[1];
					out.push_back(temp);
					indies.push_back(index);
				}
			}
		}
	}



	unsigned int i = 0;
	for (; i < out.size(); i += 3)
	{
		vert* Av, *Bv, *Cv;
		Av = &out[i];
		Bv = &out[i + 1];
		Cv = &out[i + 2];
		XMFLOAT3 Vec1, Vec2, uDir;
		XMFLOAT3 UVe1, UVe2, vDir;
		Vec1 = XMFLOAT3(Bv->pos[0] - Av->pos[0], Bv->pos[1] - Av->pos[1], Bv->pos[2] - Av->pos[2]);
		Vec2 = XMFLOAT3(Cv->pos[0] - Av->pos[0], Cv->pos[1] - Av->pos[1], Cv->pos[2] - Av->pos[2]);
		UVe1 = XMFLOAT3(Bv->uv[0] - Av->uv[0], Bv->uv[1] - Av->uv[1], 0);
		UVe2 = XMFLOAT3(Cv->uv[0] - Av->uv[0], Cv->uv[1] - Av->uv[1], 0);
		float ratio = 1.0f / (UVe1.x*UVe2.y - UVe2.x - UVe1.y);
		uDir = XMFLOAT3(
			UVe2.y * Vec1.x - UVe1.y * Vec2.x * ratio,
			UVe2.y * Vec1.y - UVe1.y * Vec2.y * ratio,
			UVe2.y * Vec1.z - UVe1.y * Vec2.z * ratio
			);
		vDir = XMFLOAT3(
			UVe1.y * Vec2.x - UVe2.y * Vec1.x * ratio,
			UVe1.y * Vec2.y - UVe2.y * Vec1.y * ratio,
			UVe1.y * Vec2.z - UVe2.y * Vec1.z * ratio
			);
		for (int c = 0; c < 3; ++c){
			///////TANGENT CALULATION///////
			XMVECTOR XMuDir, XMvDir;
			XMuDir = XMLoadFloat3(&uDir); XMvDir = XMLoadFloat3(&vDir);
			XMFLOAT3 outNorm = XMFLOAT3(out[1 + c].norm);
			XMVECTOR uNorm = XMVector3Normalize(XMuDir); XMVECTOR tNrm = XMLoadFloat3(&outNorm);
			XMVECTOR dot = XMVector3Dot(tNrm, uNorm);
			XMVECTOR Tang = XMVector3Normalize(uNorm - tNrm * dot);
			///////HANDEDNESS CALULATION///////
			XMVECTOR vNorm = XMVector3Normalize(XMvDir);
			XMVECTOR cross = XMVector3Cross(tNrm, uNorm);
			dot = XMVector3Dot(cross, vNorm);
			Tang.m128_f32[3] = (dot.m128_f32[0] < 0.0f) ? -1.0f : 1.0f;
			XMFLOAT4 newTan;
			XMStoreFloat4(&newTan, Tang);
			out[i + c].tan[0] = newTan.x;
			out[i + c].tan[1] = newTan.y;
			out[i + c].tan[2] = newTan.z;
		}
	}


	filename.pop_back();
	filename.pop_back();
	filename.pop_back();
	filename += "bin";


	ofstream bout;

	bout.open(filename.c_str(), ios_base::binary | ios_base::trunc);
	if (bout.is_open())
	{

		bout.write((char*)&numVerts, sizeof(int));

		for (size_t i = 0; i < out.size(); i++)
		{
			bout.write((char*)&out[i].pos, sizeof(float) * 3);
			bout.write((char*)&out[i].uv, sizeof(float) * 2);
			bout.write((char*)&out[i].norm, sizeof(float) * 3);
			bout.write((char*)&out[i].tan, sizeof(float) * 3);
		}
		for (size_t i = 0; i < indies.size(); i++)
		{
			bout.write((char*)&indies[i], sizeof(int));
		}
		bout.close();
	}





}

