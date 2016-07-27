#pragma once
#include <DirectXMath.h>
#include <windows.h>
#include <queue>
#include <iostream>
#include <ctime>
#include <assert.h>
#include <atlbase.h>
#include <string>
#include <fstream>
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>

#define RELEASE(point) { if(point) { point->Release(); point = nullptr; } }
#define CHECK(HR) { assert(HR == S_OK); }

#define BACKBUFFER_WIDTH	1000
#define BACKBUFFER_HEIGHT	1000
#define MSAA_SAMPLES 1;

struct PointLight
{
	XMFLOAT4 color;
	XMFLOAT4 position;
};
struct DirectionalLight
{
	XMFLOAT4 direction;
	XMFLOAT4 color;
};
struct SpotLight
{
	XMFLOAT4 position;
	XMFLOAT4 direction;
	XMFLOAT4 color;
	XMFLOAT4 spotRatio;
	XMFLOAT4 camera;
};
struct Vertex
{
	float pos[4];
	float uv[2];
	int Bones[4];
	float Weights[4];
};

struct Object
{
	XMFLOAT4X4 WorldMatrix;
};
struct Scene
{
	XMFLOAT4X4 ViewMatrix;
	XMFLOAT4X4 ProjectMatrix;
};

namespace GraphicsManager
{
	static ID3D11Device *device					= nullptr;
	static ID3D11DeviceContext *devContext		= nullptr;
	static ID3D11RenderTargetView *RTV			= nullptr;
	static ID3D11DepthStencilView *DSV			= nullptr;
	static ID3D11InputLayout *input				= nullptr;
	static ID3D11Texture2D *image				= nullptr;
	static IDXGISwapChain *swapchain			= nullptr;
	static ID3D11RasterizerState *RasterState	= nullptr;
	static ID3D11Texture2D *Zbuffer				= nullptr;
	// Viewport
	static D3D11_VIEWPORT viewport;
	// Lighting
	static PointLight		P_light;
	static DirectionalLight	D_light;
	static SpotLight		S_light;
	static ID3D11Buffer *P_lightCbuff			= nullptr;
	static ID3D11Buffer *D_lightCbuff			= nullptr;
	static ID3D11Buffer *S_lightCbuff			= nullptr;
	// Shaders
	static ID3D11VertexShader *Vertexshader		= nullptr;
	static ID3D11PixelShader *PixelShader		= nullptr;
	static ID3D11PixelShader *LightingPS		= nullptr;
	static ID3D11VertexShader *SkyboxVS			= nullptr;
	static ID3D11PixelShader *SkyboxPS			= nullptr;
};