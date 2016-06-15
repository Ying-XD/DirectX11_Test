#include "FpsClass.h"



FpsClass::FpsClass() {
}


FpsClass::~FpsClass() {
}

void FpsClass::Initialize() {
	m_fps = 0;
	m_count = 0;
	m_satrtTime = timeGetTime();
}

void FpsClass::Frame() {
	m_count += 1;
	ULONG currentTime = timeGetTime();
	if (currentTime >= m_satrtTime + 1000) {
		m_fps = m_count;
		m_count = 0;
		m_satrtTime = currentTime;
	}
}

int FpsClass::GetFPS() {
	return m_fps;
}
