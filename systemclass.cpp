////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Timer = 0;
	//m_Position = 0;
	m_Fps = 0;

	Log::GetInstance();
}


SystemClass::SystemClass(const SystemClass& other)
{
	
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// 初始化窗口API
	InitializeWindows(screenWidth, screenHeight);

	// 创建 Input对象 读入用户的键盘和鼠标输入数据.
	m_Input = new InputClass;
	if(!m_Input)
		return false;

	// 初始化 Input Class
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if(!result) {
		Log::GetInstance()->LogMsg("Could not initialize the input object.");
		return false;
	}

	// 创建 graphic 对象
	// 这个对象会处理所有图形的渲染
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)	{
		Log::GetInstance()->LogMsg("Could not create the Graphic object.");
		return false;
	}

	// 初始化 graphic 对象
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result) {
		Log::GetInstance()->LogMsg("Could not initialize the Graphic object.");
		return false;
	}
	
	m_Timer = new TimerClass;
	if(!m_Timer) {
		return false;
	}

	result = m_Timer->Initialize();
	if(!result)	{
		Log::GetInstance()->LogMsg("Could not initialize the timer object.");
		return false;
	}

	//m_Position = new PositionClass;
	//if(!m_Position) {
	//	return false;
	//}

	// 设置position的初始值，这个值将被传送 camera
	//m_Position->SetPosition(0.0f, 10.0f, -10.0f);

	// set fps
	m_Fps = new FpsClass;
	if (!m_Fps) return false;
	m_Fps->Initialize();

	return true;
}


void SystemClass::Shutdown()
{
	// Release the position object.
	//SAFE_DELETE(m_Position);
	// Release the timer object.
	SAFE_DELETE(m_Timer);
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the graphics object.
	SHUTDOWN_DELETE(m_Graphics);
	

	// Release the input object.
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
	
	return;
}


void SystemClass::Run()
{
	MSG msg = {};
	
	while(msg.message != WM_QUIT)
	{
		// 处理窗口的信息.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!Frame())
			break;
		
	}

}


bool SystemClass::Frame()
{


	// 读取用户的输入数据.
	CHECK_RESULT(m_Input->Frame());

	KeysPressed keysPressed = m_Input->GetKeysPressed();
	m_Graphics->SetKeysPressed(keysPressed);

	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsEscapePressed())
		return false;

	m_Timer->Frame();
	m_Graphics->SetFrameTime(m_Timer->GetTime());


	CHECK_RESULT(m_Graphics->Frame());

	m_Fps->Frame();


	WCHAR titleBuf[64] = { 0 };
	wsprintf(titleBuf, L"DirectX11_Test -  FPS: %d", m_Fps->GetFPS());

	SetWindowText(m_hwnd, titleBuf);
	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"DirectX11_Test";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else {
		// 设置窗口大小
		screenWidth  = SCREEN_WIDTH;
		screenHeight = SCREEN_HEIGHT;

		// 居中显示窗口
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	DWORD dStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
						    /*WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,*/
							dStyle,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	//ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
