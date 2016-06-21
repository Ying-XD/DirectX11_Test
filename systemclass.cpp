////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Timer = 0;

	//Log::GetInstance();
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

	screenWidth = 0;
	screenHeight = 0;

	// 初始化窗口API
	InitializeWindows(screenWidth, screenHeight);

	// 创建 Input对象 读入用户的键盘和鼠标输入数据.
	m_Input = new InputClass;
	CHECK_RESULT_MSG((m_Input && m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight)), "Cant not initialize InputClass.");

	// 创建 graphic 对象
	// 这个对象会处理所有图形的渲染
	m_Graphics = new GraphicsClass;
	CHECK_RESULT_MSG(m_Graphics && m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd), "Can not initialize GraphicsClass");
	
	m_Timer = new TimerClass;
	CHECK_RESULT_MSG(m_Timer && m_Timer->Initialize(), "Can not initialize TimerClass");

	return true;
}


void SystemClass::Shutdown() {
	SAFE_DELETE(m_Timer);
	SHUTDOWN_DELETE(m_Graphics);
	SAFE_DELETE(m_Input);

	ShutdownWindows();
}


void SystemClass::Run() {
	MSG msg = {};
	
	while(msg.message != WM_QUIT) {
		// 处理窗口的信息.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!Frame())
			break;
	}

}


bool SystemClass::Frame() {
	// 读取用户的输入数据.
	CHECK_RESULT(m_Input->Frame());

	m_Graphics->SetKeysPressed(m_Input->GetKeysPressed());

	if(m_Input->IsEscapePressed())
		return false;

	m_Timer->Frame();
	m_Graphics->SetFrameTime(m_Timer->GetTime());
	//m_Fps->Frame();

	CHECK_RESULT(m_Graphics->Frame());

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight) {
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
}

void SystemClass::ShutdownWindows() {

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN) {
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


LRESULT CALLBACK WndProc(HWND hWnd, UINT umessage, WPARAM wParam, LPARAM lParam)
{
	switch(umessage) {
		// Check if the window is being destroyed or closed.
		case WM_DESTROY: 
		case WM_CLOSE: 	{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default: {
			return ApplicationHandle->MessageHandler(hWnd, umessage, wParam, lParam);
		}
	}
}
