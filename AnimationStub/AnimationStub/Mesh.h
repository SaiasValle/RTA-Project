#pragma once
#include <vector>
#include "DirectXMath.h"
using namespace DirectX;

#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>
struct Vertex
{
	float pos[4];
	float uv[2];
	int Bones[4];
	float Weights[4];
};
class Mesh
{
public:
	Mesh();
	~Mesh();

	//XMFLOAT4X4* BindInverse;
	XMFLOAT4X4 ScaleMatrix, TranslationMatrix;
	std::vector<Vertex> verts;
	std::vector<unsigned int> index;
};

