#include "Renderer.h"
#include <iostream>
#include <fstream>

#define LIGHTMOVEMODIFIER 0.04f

Renderer::Renderer(HINSTANCE hinst, WNDPROC proc)
{
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"RTA Renderer", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);

	// ViewPort & SwapChain
	SetSwapChain();
	SetViewPort();
	// Set Buffers (Zbuffer, Constant, etc.)
	Initialize();
	InitializeLights();
	// Input Layouts
	D3D11_INPUT_ELEMENT_DESC vertLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};
	// Shaders
	CHECK(device->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &Vertexshader));
	CHECK(device->CreatePixelShader(Lights_PS, sizeof(Lights_PS), nullptr, &LightingPS));
	// Create Input Layout
	CHECK(device->CreateInputLayout(vertLayout, ARRAYSIZE(vertLayout), VertexShader, sizeof(VertexShader), &input));
	// Set View/Projection Matrices
	SetProjectionMatrix(scene);
	XMStoreFloat4x4(&scene.ViewMatrix, XMMatrixIdentity());
	// Meshes
	ReadScript();
}
Renderer::~Renderer()
{
	RELEASE(device);
	RELEASE(devContext);
	RELEASE(RTV);
	RELEASE(DSV);
	RELEASE(input);
	RELEASE(image);
	RELEASE(swapchain);
	RELEASE(RasterState);
	RELEASE(Zbuffer);
	RELEASE(SceneCbuffer);
	RELEASE(P_lightCbuff);
	RELEASE(D_lightCbuff);
	RELEASE(S_lightCbuff);
	RELEASE(Vertexshader);
	RELEASE(PixelShader);
	RELEASE(SkyboxVS);
	RELEASE(SkyboxPS);
	RELEASE(LightingPS);
}

bool Renderer::Run()
{
	// Render Target View
	devContext->OMSetRenderTargets(1, &RTV, DSV);
	float color[4] = { 0.4f, 0.1f, 0.1f, 1.0f };
	devContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH, 1, 0);
	devContext->ClearRenderTargetView(RTV, color);

	// Camera
	camera.CameraControl(scene.ViewMatrix);

	// Viewports
	devContext->RSSetViewports(1, &viewport);

	// Topology
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Scene
	unsigned int size;
	unsigned int offset = 0;
	D3D11_MAPPED_SUBRESOURCE map;
	size = sizeof(Scene);
	devContext->Map(SceneCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &scene, size);
	devContext->Unmap(SceneCbuffer, 0);
	devContext->VSSetConstantBuffers(1, 1, &SceneCbuffer);

	// Lighting
#if 1
	// Standard Shaders
	devContext->VSSetShader(Vertexshader, nullptr, 0);
	devContext->PSSetShader(LightingPS, nullptr, 0);
	// Standard Input Layout
	devContext->IASetInputLayout(GraphicsManager::input);
	// Point Light Constant Buffer
	size = sizeof(PointLight);
	devContext->Map(P_lightCbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &P_light, size);
	devContext->Unmap(P_lightCbuff, 0);
	devContext->PSSetConstantBuffers(1, 1, &P_lightCbuff);
	// Direction Light Constant Buffer
	size = sizeof(DirectionalLight);
	devContext->Map(D_lightCbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &D_light, size);
	devContext->Unmap(D_lightCbuff, 0);
	devContext->PSSetConstantBuffers(2, 1, &D_lightCbuff);
	// Spot Light Constant Buffer
	size = sizeof(SpotLight);
	devContext->Map(S_lightCbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &S_light, size);
	devContext->Unmap(S_lightCbuff, 0);
	devContext->PSSetConstantBuffers(3, 1, &S_lightCbuff);

	ToggleLights();
	MoveLights();
#endif

	swapchain->Present(0, 0);

	return true;
}
bool Renderer::ShutDown()
{
	// TODO: PART 1 STEP 6
	RELEASE(device);
	RELEASE(devContext);
	RELEASE(RTV);
	RELEASE(DSV);
	RELEASE(input);
	RELEASE(image);
	RELEASE(swapchain);
	RELEASE(RasterState);
	RELEASE(Zbuffer);
	RELEASE(SceneCbuffer);
	RELEASE(P_lightCbuff);
	RELEASE(D_lightCbuff);
	RELEASE(S_lightCbuff);
	RELEASE(Vertexshader);
	RELEASE(PixelShader);
	RELEASE(SkyboxVS);
	RELEASE(SkyboxPS);
	RELEASE(LightingPS);

	UnregisterClass(L"DirectXApplication", application);
	return true;
}

void Renderer::Initialize()
{
	// Image
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&image);
	device->CreateRenderTargetView(image, nullptr, &RTV);
	image->Release();
	// Raster State (Normal)
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.AntialiasedLineEnable = false;

	CHECK(device->CreateRasterizerState(&rasterDesc, &RasterState));

	// Scene Constant Buffer (View/Projection)
	SetConstBuffer(&SceneCbuffer, scene);
	// Lighting Constant Buffer
	SetConstBuffer(&P_lightCbuff, P_light);
	SetConstBuffer(&D_lightCbuff, D_light);
	SetConstBuffer(&S_lightCbuff, S_light);
	// Z buffer
	D3D11_TEXTURE2D_DESC ZbuffDesc;
	ZeroMemory(&ZbuffDesc, sizeof(ZbuffDesc));
	ZbuffDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ZbuffDesc.Format = DXGI_FORMAT_D32_FLOAT;
	ZbuffDesc.Width = BACKBUFFER_WIDTH;
	ZbuffDesc.Height = BACKBUFFER_HEIGHT;
	ZbuffDesc.MipLevels = 1;
	ZbuffDesc.ArraySize = 1;
	ZbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	ZbuffDesc.SampleDesc.Count = MSAA_SAMPLES;
	ZbuffDesc.SampleDesc.Quality = 0;

	CHECK(device->CreateTexture2D(&ZbuffDesc, 0, &Zbuffer));
	CHECK(device->CreateDepthStencilView(Zbuffer, nullptr, &DSV));
}
void Renderer::SetSwapChain()
{
	// Swapchain
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferDesc.Width = BACKBUFFER_WIDTH;
	swapDesc.BufferDesc.Height = BACKBUFFER_HEIGHT;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.OutputWindow = window;
	swapDesc.Windowed = true;
	swapDesc.SampleDesc.Count = MSAA_SAMPLES;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	CHECK(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &swapDesc, &swapchain, &device, nullptr, &devContext));
}
void Renderer::SetViewPort()
{
	// Setting Dimensions of the view
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
}
void Renderer::InitializeLights()
{
	// Point Light
	P_light.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	P_light.position = XMFLOAT4(0.0f, -1.0f, 5.0f, 0.0f);
	// Directional Light
	D_light.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	D_light.direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	// Spot Light
	S_light.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	S_light.position = XMFLOAT4(0.0f, 8.0f, 0.0f, 0.0f);
	S_light.direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	S_light.spotRatio.x = 0.98f;
	S_light.spotRatio.y = 0.8f;
}
void Renderer::ToggleLights()
{
	// Point Light
	if (GetAsyncKeyState('1') & 0x1)
	{
		if (P_light.color.x <= 0.0f)
		{
			P_light.color = XMFLOAT4(0.6f, 0.6f, 1.0f, 1.0f);
		}
		else if (P_light.color.x >= 0.1f)
			P_light.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// Directional Light
	if (GetAsyncKeyState('2') & 0x1)
	{
		if (D_light.color.w <= 0.0f)
			D_light.color = XMFLOAT4(0.6f, 0.6f, 0.4f, 1.0f);
		else if (D_light.color.w >= 0.1f)
			D_light.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// Spot Light
	if (GetAsyncKeyState('3') & 0x1)
	{
		if (S_light.color.w <= 0.0f)
			S_light.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		else if (S_light.color.w >= 0.1f)
			S_light.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}
void Renderer::MoveLights()
{
	// Point Light
#if 1
	if (GetAsyncKeyState('I'))
	{
		P_light.position.z += LIGHTMOVEMODIFIER;
	}
	if (GetAsyncKeyState('K'))
	{
		P_light.position.z += -LIGHTMOVEMODIFIER;
	}
	if (GetAsyncKeyState('J'))
	{
		P_light.position.x += -LIGHTMOVEMODIFIER;
	}
	if (GetAsyncKeyState('L'))
	{
		P_light.position.x += LIGHTMOVEMODIFIER;
	}
	if (GetAsyncKeyState('U'))
	{
		P_light.position.y += LIGHTMOVEMODIFIER;
	}
	if (GetAsyncKeyState('O'))
	{
		P_light.position.y += -LIGHTMOVEMODIFIER;
	}
	if (GetAsyncKeyState('P'))
	{
		P_light.position = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	}
#endif
	// Directional Light
#if 1
	//trans += 0.0002f;
	//if (trans > 2.0f)
	//{
	//	trans = -2.0f;
	//}
	//D_light.direction = XMFLOAT4(trans, -1.0f, 0.0f, 0.0f);
#endif
	// Spot Light
#if 1
	S_light.position.x = camera.GetCameraMat()._41;
	S_light.position.y = camera.GetCameraMat()._42;
	S_light.position.z = camera.GetCameraMat()._43;

	S_light.direction.x = camera.GetCameraMat()._31;
	S_light.direction.y = camera.GetCameraMat()._32;
	S_light.direction.z = camera.GetCameraMat()._33;
#endif
}
void Renderer::SetProjectionMatrix(Scene& wvp)
{
	float zNear = 0.1f;
	float zFar = 100.0f;
	float Yscale = 1 / (tanf((65 >> 1) * (3.1415f / 180.0f)));
	float Xscale = Yscale * 1;

	XMStoreFloat4x4(&wvp.ProjectMatrix, XMMatrixIdentity());
	wvp.ProjectMatrix._11 = Xscale;
	wvp.ProjectMatrix._22 = Yscale;
	wvp.ProjectMatrix._33 = zFar / (zFar - zNear);
	wvp.ProjectMatrix._34 = 1.0f;
	wvp.ProjectMatrix._43 = -(zFar * zNear) / (zFar - zNear);
	wvp.ProjectMatrix._44 = 0.0f;
}
void Renderer::ReadScript(char *filename)
{
	FILE *file;
	char buffer[256] = { 0 };

	fopen_s(&file, filename, "r");

	if (file)
	{
		vector<string> filenames;
		vector<string> textures;
		vector<string> normalmaps;
		vector<string> specularmaps;

		int nummodels;
		fscanf_s(file, "%i", &nummodels);

		if (nummodels > 0)
		{
			for (UINT i = 0; i < nummodels; i++)
			{
				fscanf(file, "%s", buffer);

				if (strcmp(buffer, "#") == 0)
				{
					string modelname;
					fscanf_s(file, "%s\n" &modelname);
					filenames.push_back(modelname);
				}
				if (strcmp(buffer, "T") == 0)
				{
					string texturename;
					fscanf_s(file, "%s\n", &texturename);
					textures.push_back(texturename);
				}
				if (strcmp(buffer, "N") == 0)
				{
					string normalname;
					fscanf_s(file, "%s\n", &normalname);
					normalmaps.push_back(normalname);
				}
				if (strcmp(buffer, "S") == 0)
				{
					string specularname;
					fscanf_s(file, "%s\n", &specularname);
					specularmaps.push_back(specularname);
				}
			}
		}
	}
}

template <typename Type>
void Renderer::SetVertBuffer(ID3D11Buffer **vertbuff, vector<Type> verts)
{
	// Vertex buffer
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = verts.data();
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	D3D11_BUFFER_DESC vertbuffDesc;
	ZeroMemory(&vertbuffDesc, sizeof(vertbuffDesc));
	vertbuffDesc.ByteWidth = sizeof(Type) * verts.size();
	vertbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertbuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	vertbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&vertbuffDesc, &data, vertbuff));
}
template <typename Type>
void Renderer::SetIndexBuffer(ID3D11Buffer **indexbuff, vector<Type> indices)
{
	// Index buffer
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indices.data();
	data.SysMemPitch = NULL;
	data.SysMemSlicePitch = NULL;

	D3D11_BUFFER_DESC IbuffDesc;
	ZeroMemory(&IbuffDesc, sizeof(IbuffDesc));
	IbuffDesc.ByteWidth = sizeof(Type) * indices.size();
	IbuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IbuffDesc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
	IbuffDesc.StructureByteStride = 0;

	CHECK(device->CreateBuffer(&IbuffDesc, &data, indexBuff));
}
template <typename Type>
void Renderer::SetConstBuffer(ID3D11Buffer **constbuff, Type size)
{
	// Constant Buffer
	D3D11_BUFFER_DESC ConstbuffDesc;
	ZeroMemory(&ConstbuffDesc, sizeof(ConstbuffDesc));
	ConstbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstbuffDesc.ByteWidth = sizeof(Type);

	CHECK(device->CreateBuffer(&ConstbuffDesc, nullptr, constbuff));
}

// Windows Application
#if 1 
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	Renderer myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
						break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif