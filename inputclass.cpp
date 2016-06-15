////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	
	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 鼠标初始位置.
	m_mouseX = 0;
	m_mouseY = 0;

	// Initialize the main direct input interface.
	CHECK_HRESULT(
		DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL)
	);

	// ---- 初始化 键盘 输入接口 ----
	CHECK_HRESULT(
		m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL)
	);


	// ---- keyboard 设置 ----
	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	CHECK_HRESULT(
		m_keyboard->SetDataFormat(&c_dfDIKeyboard)
	);

	// Set the cooperative level of the keyboard to not share with other programs.
	CHECK_HRESULT(
		m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)
	);

	// Now acquire the keyboard.
	CHECK_HRESULT(
		m_keyboard->Acquire()
	);


	// --- 初始化 鼠标 输入接口 ---
	CHECK_HRESULT(
		m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL)
	);

	// Set the data format for the mouse using the pre-defined mouse data format.
	CHECK_HRESULT(
		m_mouse->SetDataFormat(&c_dfDIMouse)
	);

	// Set the cooperative level of the mouse to share with other programs.
	CHECK_HRESULT(
		m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)
	);
	
	// Acquire the mouse.
	CHECK_HRESULT(
		m_mouse->Acquire()
	);

	return true;
}


void InputClass::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}


bool InputClass::Frame()
{
	

	//  读取 keyboard 状态
	CHECK_RESULT(ReadKeyboard());


	// 读取鼠标状态
	CHECK_RESULT(ReadMouse());

	// Process the changes in the mouse and keyboard.
	ProcessInput();


	SetKeyPress();
	
	return true;
}


bool InputClass::ReadKeyboard()
{
	HRESULT hr;

	// Read the keyboard device.
	hr= m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(hr)) 	{
		// 如果是 lost focus 或者 not acquired 就重新尝试获得
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}
		
	return true;
}


bool InputClass::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


void InputClass::ProcessInput()
{
	// 修改鼠标的坐标.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 限制鼠标范围
	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }
	
	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
	
}

void InputClass::SetKeyPress() {
	for (size_t i = 0;i < KEYS_NUM; ++i) {
		m_keyPressed._key[i] = (m_keyboardState[KeyState[i]] & 0x80) ? true : false;
	}
	
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}


bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	return (m_keyboardState[DIK_ESCAPE] & 0x80);
}


bool InputClass::IsSpacePressed() {
	return m_keyPressed.space;
}

KeysPressed InputClass::GetKeysPressed() {
	KeysPressed keyPressed;
	for (size_t i = 0; i < KEYS_NUM;i++) {
		keyPressed._key[i] = m_keyPressed._key[i];
	}
	return keyPressed;
}
