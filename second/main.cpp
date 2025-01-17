#include <windows.h>
#include <d3d11.h>
#include <DxErr.h>
#include <D3DX11.h>

HINSTANCE g_hInstance = NULL;
HWND g_hWnd = NULL;
LPCWSTR g_name = L"FirstD3D11Demo";
D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;                //驱动类型
D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;            //特征等级    
ID3D11Device *g_pd3dDevice = NULL;                                    //设备
ID3D11DeviceContext *g_pImmediateContext = NULL;                    //设备上下文
IDXGISwapChain *g_pSwapChain = NULL;                                //交换链
ID3D11RenderTargetView *g_pRenderTargetView = NULL;                    //要创建的视图


HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	if (FAILED(InitWindow(hInstance, nShowCmd)))
		return 0;
	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	//消息处理
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else//渲染
		{
			Render();
		}
	}
	CleanupDevice();
	return static_cast<int>(msg.wParam);
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hIconSm = wcex.hIcon;
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WndProc;
	wcex.lpszClassName = g_name;
	wcex.lpszMenuName = NULL;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	g_hInstance = hInstance;
	RECT rc{ 0,0,640,480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindowEx(WS_EX_APPWINDOW, g_name, g_name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, g_hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wPararm, LPARAM lParam)
{
	//处理消息事件
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wPararm, lParam);
	}
	return 0;
}

//创建设备及交换链
HRESULT InitDevice()
{
	HRESULT hResult = S_OK;//返回结果

	RECT rc;			//存放窗口数据
	GetClientRect(g_hWnd, &rc);//获取窗口客户区大小
	UINT width = rc.right - rc.left;	
	UINT height = rc.bottom - rc.top;	

	UINT createDeviceFlags = 0;		//设置创建格式，一般用DEBUG
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//驱动类型数组
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	//特征级别数组,从高到低
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//设置交换链
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));//填充
	sd.BufferCount = 1;                              //我们只创建一个后缓冲（双缓冲）因此为1
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;                      //1重采样
	sd.SampleDesc.Quality = 0;                      //采样等级
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;      //常用参数
	sd.Windowed = TRUE;                              //是否全屏

	//用于初始化一个D3D设备，带有swapchain
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hResult = D3D11CreateDeviceAndSwapChain(
			NULL,                                //默认图形适配器
			g_driverType,                        //驱动类型
			NULL,                                //实现软件渲染设备的动态库句柄，如果使用的驱动设备类型是软件设备则不能为NULL
			createDeviceFlags,                    //创建标志，0用于游戏发布，一般D3D11_CREATE_DEVICE_DEBUG允许我们创建可供调试的设备，在开发中比较有用
			featureLevels,                        //特征等级
			numFeatureLevels,                    //特征等级数量
			D3D11_SDK_VERSION,                    //sdk版本号
			&sd,				//	交换链
			&g_pSwapChain,			
			&g_pd3dDevice,				//d3d设备
			&g_featureLevel,			//特征等级
			&g_pImmediateContext		//d3d描述表
		);
		if (SUCCEEDED(hResult))	//创建成功就退出
			break;
	}
	if (FAILED(hResult))		//判断到底是怎么退出的
		return hResult;

//创建渲染目标视图

	ID3D11Texture2D *pBackBuffer = NULL;
	//获取交换链中后缓冲区地址，	把后缓冲区的资源放在pBackBuffer里
	hResult = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hResult))
		return hResult;

	//创建目标视图，把pBackBuffer对应的资源放在新建的视图上
	hResult = g_pd3dDevice->CreateRenderTargetView(pBackBuffer,// 视图对应资源
		NULL,	//视图描述
		&g_pRenderTargetView);//要创建的视图的地址
	//释放后缓冲
	pBackBuffer->Release();
	if (FAILED(hResult))
		return hResult;

	//绑定到渲染管线，把刚刚创建的视图放到管线上渲染
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

	//设置viewport
	D3D11_VIEWPORT vp;
	vp.Height = (FLOAT)height;		
	vp.Width = (FLOAT)width;		
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}

void Render()
{
	//定义颜色
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; //red,green,blue,alpha
	//用特定颜色清除屏幕
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	//
	g_pSwapChain->Present(0, 0);
}

void CleanupDevice()
{
	if (g_pImmediateContext)
		g_pImmediateContext->ClearState();
	if (g_pSwapChain)
		g_pSwapChain->Release();
	if (g_pRenderTargetView)
		g_pRenderTargetView->Release();
	if (g_pImmediateContext)
		g_pImmediateContext->Release();
	if (g_pd3dDevice)
		g_pd3dDevice->Release();
}