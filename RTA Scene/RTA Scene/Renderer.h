#pragma once
#include "GraphicsManager.h"
#include "Camera.h"
#include "Mesh.h"
#include "Lights_PS.csh"
#include "VertexShader.csh"
using namespace GraphicsManager;

class Renderer
{
private:
	HINSTANCE application;
	WNDPROC	appWndProc;
	HWND window;

	ID3D11Buffer *SceneCbuffer = nullptr;
	Scene scene;
	Camera camera;

public:
	Renderer(HINSTANCE hinst, WNDPROC proc);
	Renderer();
	~Renderer();

	bool Run();
	bool ShutDown();

	void Initialize();
	void SetSwapChain();
	void SetViewPort();
	void InitializeLights();
	void ToggleLights();
	void MoveLights();
	void SetProjectionMatrix(Scene& wvp);


	template <typename Type>
	void SetVertBuffer(ID3D11Buffer **vertbuff, vector<Type> verts);
	template <typename Type>
	void SetIndexBuffer(ID3D11Buffer **indexbuff, vector<Type> indices);
	template <typename Type>
	void SetConstBuffer(ID3D11Buffer **constbuff, Type size);
};