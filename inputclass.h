//	------ InputClass	--------
#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


#include <dinput.h>
#include "global.h"


class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);

	bool IsEscapePressed();

	bool IsSpacePressed();
	KeysPressed GetKeysPressed();
private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();
	void SetKeyPress();
private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

	//const UCHAR KeyState[KEYS_NUM] = {
	//	DIK_W, DIK_A, DIK_S, DIK_D,
	//	DIK_LEFT, DIK_RIGHT, DIK_UP, DIK_DOWN,
	//	DIK_PGUP, DIK_PGDN,
	//	DIK_ESCAPE, DIK_SPACE,
	//	DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9,
	//	DIK_HOME, DIK_END,
	//	DIK_TAB
	//};
	KeysPressed m_keyPressed;


};

