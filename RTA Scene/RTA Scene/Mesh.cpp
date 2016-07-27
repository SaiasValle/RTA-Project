#include "Mesh.h"


Mesh::Mesh()
{
	Object temp;

	XMMATRIX M = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixScaling(0.2, 0.2, 0.2));
	XMStoreFloat4x4(&temp.WorldMatrix, M);

	Model.push_back(temp);
	instances = 1;
	for (UINT i = 0; i < 3; i++)
	{
		m_SRV[i] = nullptr;
	}
}
Mesh::~Mesh()
{
	if (Vertbuffer && Indexbuffer && m_SRV[0])
	{
		RELEASE(Vertbuffer);
		RELEASE(Indexbuffer);
		RELEASE(Constbuffer);
		RELEASE(Constbuffer2);
		RELEASE(m_SRV[0]);
		RELEASE(m_SRV[1]);
		RELEASE(m_SRV[2]);
	}
	verts.clear();
	index.clear();
	delete[] bindinverse;
}

void Mesh::TranslateModel(XMFLOAT3 posVector)
{
	XMMATRIX trans = XMMatrixTranslation(posVector.x, posVector.y, posVector.z);
	XMStoreFloat4x4(&Model[0].WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&Model[0].WorldMatrix), trans));
}
void Mesh::RotateModel(XMFLOAT3 rotXYZ)
{
	XMMATRIX worldmat = XMLoadFloat4x4(&Model[0].WorldMatrix);

	XMMATRIX rotate = XMMatrixRotationX(rotXYZ.x);
	worldmat = XMMatrixMultiply(worldmat, rotate);

	rotate = XMMatrixRotationY(rotXYZ.y);
	worldmat = XMMatrixMultiply(worldmat, rotate);

	rotate = XMMatrixRotationZ(rotXYZ.z);
	worldmat = XMMatrixMultiply(worldmat, rotate);

	XMStoreFloat4x4(&Model[0].WorldMatrix, worldmat);
}
void Mesh::ScaleModel(XMFLOAT3 scale)
{
	XMMATRIX worldmat = XMLoadFloat4x4(&Model[0].WorldMatrix);

	XMMATRIX Scale = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
	worldmat = XMMatrixMultiply(worldmat, Scale);

	XMStoreFloat4x4(&Model[0].WorldMatrix, worldmat);
}
void Mesh::LoadTextureDDS(wchar_t *textureName, ID3D11Device *device, int i)
{
	HRESULT hr = CreateDDSTextureFromFile(device, textureName, nullptr, &m_SRV[i]);
	if (FAILED(hr))
		return;
}
void Mesh::LoadFromOBJ(char *filename, ID3D11Device *device)
{
	/*
	vector<XMFLOAT4> position;
	vector<XMFLOAT2> uv;
	vector<XMFLOAT3> normals;
	vector<unsigned int> posIndex;
	vector<unsigned int> uvIndex;
	vector<unsigned int> normIndex;
	vector<Vertex> verts;
	char buffer[256] = { 0 };
	FILE *file;

	fopen_s(&file, filename, "r");

	if (file != nullptr)
	{
	for (;;)
	{
	int end = fscanf(file, "%s", buffer);
	if (end == EOF)
	break;

	if (strcmp(buffer, "v") == 0)
	{
	float x = 0, y = 0, z = 0;
	fscanf_s(file, "%f %f %f\n", &x, &y, &z);
	position.push_back(XMFLOAT4(x, y, z, 1.0f));
	}
	if (strcmp(buffer, "vt") == 0)
	{
	float u = 0, v = 0;
	fscanf_s(file, "%f %f\n", &u, &v);
	uv.push_back(XMFLOAT2(u, v));
	}
	if (strcmp(buffer, "vn") == 0)
	{
	float x = 0, y = 0, z = 0;
	fscanf_s(file, "%f %f %f\n", &x, &y, &z);
	normals.push_back(XMFLOAT3(x, y, z));
	}
	if (strcmp(buffer, "f") == 0)
	{
	unsigned int posI[3], uvI[3], normI[3];
	fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
	&posI[0], &uvI[0], &normI[0],
	&posI[1], &uvI[1], &normI[1],
	&posI[2], &uvI[2], &normI[2]);

	posIndex.push_back(posI[0]);
	posIndex.push_back(posI[1]);
	posIndex.push_back(posI[2]);

	uvIndex.push_back(uvI[0]);
	uvIndex.push_back(uvI[1]);
	uvIndex.push_back(uvI[2]);

	normIndex.push_back(normI[0]);
	normIndex.push_back(normI[1]);
	normIndex.push_back(normI[2]);
	}
	}
	vector<unsigned int> index2;

	for (unsigned int i = 0; i < (unsigned int)posIndex.size(); i++)
	{
	Vertex tempVert;

	tempVert.x = position[posIndex[i] - 1].x;
	tempVert.y = position[posIndex[i] - 1].y;
	tempVert.z = position[posIndex[i] - 1].z;
	tempVert.w = 1;

	tempVert.color[0] = 0.0f;
	tempVert.color[1] = 0.0f;
	tempVert.color[2] = 0.0f;

	tempVert.uv[0] = uv[uvIndex[i] - 1].x;
	tempVert.uv[1] = 1 - uv[uvIndex[i] - 1].y;

	tempVert.normal[0] = normals[normIndex[i] - 1].x;
	tempVert.normal[1] = normals[normIndex[i] - 1].y;
	tempVert.normal[2] = normals[normIndex[i] - 1].z;

	verts.push_back(tempVert);
	index2.push_back(i);
	}

	// Initialize Buffers
	numIndices = index2.size();
	Initialize(device, &Vertbuffer, verts, &Indexbuffer, index2);
	}

	return;
	*/
}
void Mesh::LoadFromFBX(char *filename, ID3D11Device *device)
{
	/*
	vector<Vertex> verts;
	vector<unsigned int> index;
	ifstream bin;
	bin.open(filename, ios_base::binary);
	if (bin.is_open())
	{
	int numverts;
	bin.read((char*)&numverts, sizeof(int));
	for (size_t i = 0; i < numverts; i++)
	{
	Vertex vert;
	bin.read((char*)&vert.x, sizeof(float));
	bin.read((char*)&vert.y, sizeof(float));
	bin.read((char*)&vert.z, sizeof(float));
	vert.w = 1;
	bin.read((char*)&vert.uv, sizeof(float) * 2);
	vert.uv[1] = 1 - vert.uv[1];
	bin.read((char*)&vert.normal, sizeof(float) * 3);
	bin.read((char*)&vert.tangent, sizeof(float) * 3);
	bool found = false;
	size_t c = 0;
	for (; c < verts.size(); c++)
	{
	if (verts[c].x == vert.x && verts[c].y == vert.y && verts[c].z == vert.z)
	if (verts[c].normal == vert.normal)
	{
	found = true;
	break;
	}
	}
	if (found)
	{
	index.push_back(c);
	continue;
	}
	else
	{
	verts.push_back(vert);
	index.push_back(verts.size() - 1);
	}
	}
	numIndices = index.size();
	Initialize(device, &Vertbuffer, verts, &Indexbuffer, index);
	}
	*/
}
void Mesh::Initialize(ID3D11Device *device, ID3D11Buffer **vertbuff, vector<Vertex> verts, ID3D11Buffer **indexBuff, vector<unsigned int> indices)
{
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = verts.data();
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	// Vertex buffer
	D3D11_BUFFER_DESC ObjbuffDesc;
	ZeroMemory(&ObjbuffDesc, sizeof(ObjbuffDesc));
	ObjbuffDesc.ByteWidth = sizeof(Vertex) * verts.size();
	ObjbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ObjbuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ObjbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	ObjbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&ObjbuffDesc, &data, vertbuff));

	// Index buffer
	data.pSysMem = indices.data();
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	D3D11_BUFFER_DESC IbuffDesc;
	ZeroMemory(&IbuffDesc, sizeof(IbuffDesc));
	IbuffDesc.ByteWidth = sizeof(unsigned int) * indices.size();
	IbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	IbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&IbuffDesc, &data, indexBuff));

	// Mesh Constant Buffer
	D3D11_BUFFER_DESC ConstbuffDesc;
	ZeroMemory(&ConstbuffDesc, sizeof(ConstbuffDesc));
	ConstbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ConstbuffDesc.ByteWidth = sizeof(XMMATRIX)*52;

	CHECK(device->CreateBuffer(&ConstbuffDesc, nullptr, &Constbuffer));
	ConstbuffDesc.ByteWidth = sizeof(XMFLOAT4X4) * 52;
	CHECK(device->CreateBuffer(&ConstbuffDesc, nullptr, &Constbuffer2));
}
void Mesh::Draw(ID3D11DeviceContext *context)
{
	unsigned int size;
	unsigned int offset = 0;


	//D3D11_MAPPED_SUBRESOURCE map;
	//// Constant Buffer
	//context->Map(Constbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);

	//if (instances == 1)
	//{
	//	size = sizeof(Object);
	//}
	//if (instances > 1)
	//{
	//	size = sizeof(Object) * instances;
	//}

	//memcpy(map.pData, Model.data(), size);
	//context->Unmap(Constbuffer, 0);
	//context->VSSetConstantBuffers(0, 1, &Constbuffer);


	// Vertex Buffer
	size = sizeof(Vertex);
	context->IASetVertexBuffers(0, 1, &Vertbuffer, &size, &offset);
	context->IASetIndexBuffer(Indexbuffer, DXGI_FORMAT_R32_UINT, offset);
	// Topology
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Texture
	for (UINT i = 0; i < 3; i++)
	{
		if (m_SRV[i])
		{
			context->PSSetShaderResources(i, 1, &m_SRV[i]);
		}
	}
	// Draw
	if (instances == 1)
		context->DrawIndexed(numIndices, 0, 0);
	if (instances > 1)
		context->DrawIndexedInstanced(numIndices, instances, 0, 0, 0);
}
