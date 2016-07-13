#pragma once
#include "GraphicsManager.h"

class Mesh
{
private:
	vector<Object> Model;
	ID3D11Buffer *Constbuffer		= nullptr;
	ID3D11Buffer *Vertbuffer		= nullptr;
	ID3D11Buffer *Indexbuffer		= nullptr;
	unsigned int numIndices			= 0;
	unsigned int instances			= 1;
	// Texture
	ID3D11ShaderResourceView *m_SRV[3];

public:
	Mesh();
	~Mesh();

	XMFLOAT4X4* GetWorldMatrix(UINT index) { return &Model[index].WorldMatrix; }
	void SetWorldMatrix(XMFLOAT4X4 rhs, UINT index) { Model[index].WorldMatrix = rhs; }
	void SetInstances(UINT numinst) { instances = numinst; }
	UINT GetInstances() { return instances; }
	ID3D11ShaderResourceView** GetSRV(UINT i) { return &m_SRV[i]; }

	void TranslateModel(XMFLOAT3 posVector);
	void RotateModel(XMFLOAT3 rotXYZ);
	void ScaleModel(XMFLOAT3 scale);

	void LoadTextureDDS(wchar_t *filename, ID3D11Device *device);
	void LoadFromOBJ(char *filename, ID3D11Device *device);
	void LoadFromFBX(char *filename, ID3D11Device *device);
	template<typename Type>
	void Initialize(ID3D11Device *device, ID3D11Buffer **vertbuff, vector<Type> verts, ID3D11Buffer **indexBuff, vector<unsigned int> indices);
	void Draw(ID3D11DeviceContext *context);
};

