#pragma once
#pragma comment(lib,"winmm.lib")

#include <Windows.h>
#include <mmsystem.h>

class FpsClass {
public:
	FpsClass();
	~FpsClass();
	void Initialize();
	void Frame();
	int GetFPS();
private:
	int m_fps,m_count;
	ULONG m_satrtTime;
};

