#include "stdafx.h"
#include "Input/Controller.h"
#include "Screen/ScreenManager.h"
#include "Sound/SoundManager.h"
#include "Screen/PlayScreen/Graphics/ModelManager.h"
#include <ctime> 
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HWND					g_hWndMain				= NULL;

IDXGISwapChain*         g_SwapChain				= NULL;
ID3D11RenderTargetView* g_RenderTargetView		= NULL;

ID3D11Texture2D*        g_DepthStencil			= NULL;
ID3D11DepthStencilView* g_DepthStencilView		= NULL;
ID3D11Device*			g_Device				= NULL;
ID3D11DeviceContext*	g_DeviceContext			= NULL;

/*
	PROJECT RELATED FIELDS
*/
ScreenData*				ScreenSetupData			=	NULL;
ScreenManager*			ScreenM					=	NULL;
int						Settings[6];


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
bool				InitWndApp(HINSTANCE hInstanceHandle, int show);
LRESULT CALLBACK	WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int					Run();
HRESULT				Render(float deltaTime);
HRESULT				Update(float deltaTime);
HRESULT				InitDirect3D();
char*				FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel);

//Main
void LoadSettings()
{
	//	Pre settings
	Settings[0]		=	1280;
	Settings[1]		=	720;
	Settings[2]		=	0;
	Settings[3]		=	10;
	Settings[4]		=	10;
	Settings[5]		=	10;

	ifstream	tFileStream;

	//	Try and open the file
	tFileStream.open( "DATA/Settings.cfg" );
			
	//	If we can't open the file,
	//	create it
	if ( !tFileStream.is_open() )
	{
		ofstream settingsFile;
		settingsFile.open( "DATA/Settings.cfg" );

		settingsFile << "Width 1280" << endl;
		settingsFile << "Height 720" << endl;
		settingsFile << "Fullscreen 0" << endl;
		settingsFile << "Master 10" << endl;
		settingsFile << "Song 10" << endl;
		settingsFile << "SFX 10" << endl;

		settingsFile.close();
		return;
	}
	string	ResX, ResY, Fullscreen, Master, SFX, Song;
	getline(tFileStream, ResX);
	getline(tFileStream, ResY);
	getline(tFileStream, Fullscreen);
	getline(tFileStream, Master);
	getline(tFileStream, Song);
	getline(tFileStream, SFX);
	tFileStream.close();

	if( ResX == "" || ResY == "" || Fullscreen == "" || Master == "" || SFX == "" || Song == "" )
		return;

	ResX		=	ResX.substr( ResX.find(' ') );
	ResY		=	ResY.substr( ResY.find(' ') );
	Fullscreen	=	Fullscreen.substr( Fullscreen.find(' ') );
	Master		=	Master.substr( Master.find(' ') );
	SFX			=	SFX.substr( SFX.find(' ') );
	Song		=	Song.substr( Song.find(' ') );

	Settings[0]		=	atoi( ResX.c_str() );
	Settings[1]		=	atoi( ResY.c_str() );
	Settings[2]		=	atoi( Fullscreen.c_str() );
	Settings[3]		=	atoi( Master.c_str() );
	Settings[4]		=	atoi( SFX.c_str() );
	Settings[5]		=	atoi( Song.c_str() );

}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	AllocConsole();
	LoadSettings();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

	if (!InitWndApp(hInstance, nShowCmd))
	{
		::MessageBox(0, "Initalaization Failed", "Error", MB_OK);
		return 0;
	}
	if( FAILED( InitDirect3D() ) )
	{
		::MessageBox(0, "Failed to initalize Direct3D", "Error", MB_OK);
		return 0;
	}
	LPRECT Rect = new RECT();
	GetWindowRect(g_hWndMain, Rect);
	int centerX = (Rect->left + Rect->right) / 2;
	int centerY = (Rect->top + Rect->bottom) / 2;
	SetCursorPos(centerX, centerY);
	ShowCursor(true);

	ScreenSetupData	=	new ScreenData();
	ScreenSetupData->WIDTH		=	Settings[0];
	ScreenSetupData->HEIGHT		=	Settings[1];
	ScreenSetupData->FULLSCREEN	=	Settings[2] == 1;

	ScreenSetupData->DEPTH_STENCIL		=	g_DepthStencil;
	ScreenSetupData->DEPTH_STENCIL_VIEW	=	g_DepthStencilView;

	ScreenSetupData->DEVICE				=	g_Device;
	ScreenSetupData->DEVICE_CONTEXT		=	g_DeviceContext;

	ScreenSetupData->SWAP_CHAIN			=	g_SwapChain;
	ScreenSetupData->RENDER_TARGET_VIEW	=	g_RenderTargetView;
	
	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(g_Device, L"Verdana", &ScreenSetupData->TEXT_INSTANCE);
	pFW1Factory->Release();

	DebugScreen::Initialize(ScreenSetupData);

	InputManager::Initialize(&hInstance, &g_hWndMain, Settings[0], Settings[1]);

	SoundManager::GetInstance();
	SoundManager::GetInstance()->SetVolume( Master, 0.1f * Settings[3] );
	SoundManager::GetInstance()->SetVolume( Song, 0.1f * Settings[4] );
	SoundManager::GetInstance()->SetVolume( SFX, 0.1f * Settings[5] );
	
	ModelManager::Initialize(g_Device);
	
	ScreenM	=	new ScreenManager(ScreenSetupData);

	ScreenM->ChangeScreen(MAIN_MENU_SCREEN);


	return Run();
}

bool InitWndApp(HINSTANCE hInstanceHandle, int show)
{
	WNDCLASS wc;
	wc.style				= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc			= WndProc;
	wc.cbClsExtra			= 0;
	wc.cbWndExtra			= 0;
	wc.hInstance			= hInstanceHandle;
	wc.hIcon				= ::LoadIconA(0, IDI_APPLICATION);
	wc.hCursor				= ::LoadCursor(0, IDC_ARROW);
	wc.hbrBackground		= static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName			= 0;
	wc.lpszClassName		= "Window";

	if (!RegisterClass(&wc))
	{
		::MessageBox(0, "Failed to register WNDCLASS", 0, MB_OK);
		return 0;
	}

	DWORD	wStyle	=	WS_POPUP;

	RECT rc = { 0, 0, Settings[0], Settings[1] };
	AdjustWindowRect( &rc, wStyle, FALSE );

	g_hWndMain = ::CreateWindow("Window", 
							"Codename: Gamma", 
							wStyle,
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							rc.right - rc.left,
							rc.bottom - rc.top,
							0, 
							0, 
							hInstanceHandle,
							0);

	if (g_hWndMain == 0)
	{
		::MessageBox(0, "Failed to create WNDCLASS", 0, MB_OK);
		return 0;
	}
	

	::ShowWindow(g_hWndMain, show);
	::UpdateWindow(g_hWndMain);

	return true;
}

HRESULT InitDirect3D()
{
	HRESULT hr = S_OK;;

	int screenWidth = Settings[0];
	int screenHeight = Settings[1];

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverType;

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]); //same as ARRAYSIZE(x) macro

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = screenWidth;
	sd.BufferDesc.Height = screenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWndMain;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = Settings[2] == 0;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;
	
	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&g_SwapChain,
			&g_Device,
			&initiatedFeatureLevel,
			&g_DeviceContext);

		if( SUCCEEDED( hr ) )
			break;
	}
	
	if( FAILED(hr) )
		return hr;

	ID3D11Texture2D* pBackBuffer;
	hr = g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;	
	
	hr = g_Device->CreateRenderTargetView( pBackBuffer, NULL, &g_RenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_Device->CreateTexture2D( &descDepth, NULL, &g_DepthStencil );
	if( FAILED(hr) )
		return hr;


	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_Device->CreateDepthStencilView( g_DepthStencil, &descDSV, &g_DepthStencilView );
	if( FAILED(hr) )
		return hr;


	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)screenWidth;
	vp.Height = (float)screenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_DeviceContext->RSSetViewports( 1, &vp );

	return S_OK;
}

//Uppdaterar världen
bool firstUpdate;
int Run()
{
	firstUpdate = true;
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;

	while (WM_QUIT != msg.message)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);

			if (firstUpdate)
			{
				prevTimeStamp = currTimeStamp;
				firstUpdate = false;
			}

			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			Update(dt);
			Render(dt);

			prevTimeStamp = currTimeStamp;
		}
	}

	return (int)msg.wParam;
}

void CloseApplication()
{
	Effects::DestroyAll();
	RenderStates::DestroyAll();
	InputLayouts::DestroyAll();

	if( ScreenSetupData->TEXT_INSTANCE )	ScreenSetupData->TEXT_INSTANCE->Release();

	ScreenSetupData	=	NULL;

	if ( ScreenM )	delete	ScreenM;

	if ( SoundManager::GetInstance() )	delete	SoundManager::GetInstance();
	if ( InputManager::GetInstance() )	delete	InputManager::GetInstance();
	if ( ModelManager::GetInstance() )	delete	ModelManager::GetInstance();

	//	Clear all created assets
	if(g_SwapChain)			g_SwapChain->Release();
	if(g_RenderTargetView)	g_RenderTargetView->Release();
	if(g_DepthStencil)		g_DepthStencil->Release();
	if(g_DepthStencilView)	g_DepthStencilView->Release();
	if(g_Device)			g_Device->Release();
	if(g_DeviceContext)		g_DeviceContext->Release();

	g_SwapChain			=	0;
	g_RenderTargetView	=	0;
	g_DepthStencil		=	0;
	g_DepthStencilView	=	0;
	g_Device			=	0;
	g_DeviceContext		=	0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			switch (wParam)
			{
				//case VK_ESCAPE:					//Avslutar programmet om man trycker på escape.
					//CloseApplication();
					//::DestroyWindow(hWnd);
			}
			return 0;
	}
	
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

HRESULT Update(float deltaTime)
{
	ScreenM->Update(deltaTime);

	if(ScreenM->CloseGame())
	{
		CloseApplication();
		::DestroyWindow(g_hWndMain);
		return S_OK;
	}

	/*
	system("cls");
	cout << "Left Trigger: " << Control->GetTriggerValue(LEFT) << endl;
	cout << "Right Trigger: " << Control->GetTriggerValue(RIGHT) << endl;
	cout << "-------------" << endl;

	cout << "Up: " << Control->GetButtonState(D_UP) << endl;
	cout << "Left: " << Control->GetButtonState(D_LEFT) << endl;
	cout << "Right: " << Control->GetButtonState(D_RIGHT) << endl;
	cout << "Down: " << Control->GetButtonState(D_DOWN) << endl;

	cout << "-------------" << endl;
	cout << "Left Stick: (" << Control->GetStickDirection(LEFT).x << ", " << Control->GetStickDirection(LEFT).y <<  "), " << Control->GetStickLength(LEFT) << endl;
	cout << "Right Stick: (" << Control->GetStickDirection(RIGHT).x << ", " << Control->GetStickDirection(RIGHT).y <<  "), " << Control->GetStickLength(RIGHT) << endl;
	*/



	return S_OK;
}

HRESULT Render(float deltaTime)
{
	if( !g_DeviceContext )
		return S_OK;

	g_DeviceContext->ClearDepthStencilView( g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	g_DeviceContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );

	ScreenM->Render();
	
	/*
	char title[255];
	sprintf_s(title, sizeof(title), "FPS: %d",
		(int)(1.0f / deltaTime));

	SetWindowText(g_hWndMain, title);
	*/

	//Växlar rendertarget så att bilden visas på skärmen.
	if(FAILED(g_SwapChain->Present( 0, 0 )))
		return E_FAIL;

	return S_OK;
}

char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
{
	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
		return "11.0";
	if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		return "10.1";
	if(featureLevel == D3D_FEATURE_LEVEL_10_0)
		return "10.0";

	return "Unknown";
}