#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	SystemClass* System;
	
	System = new SystemClass;
	if (!System || !System->Initialize()) {
		return 0;
	}

	System->Run();

	SHUTDOWN_DELETE(System);

	return 0;
}