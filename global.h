//----------------------------------
//	global.h
//----------------------------------
#pragma once
#include <Windows.h>
#include <dinput.h>

//#define	DEBUG
#define TESSELLATION		TRUE
#define FULL_SCREEN			FALSE
#define	VSYNC_ENABLED		FALSE

#define SCREEN_WIDTH		1200
#define SCREEN_HEIGHT		900
#define SCREEN_FAR			100.0f
#define SCREEN_NEAR			1.0f

#define SHADOWMAP_WIDTH		(512)
#define SHADOWMAP_HEIGHT	SHADOWMAP_WIDTH
#define LIGHTVIEW_NEAR		1.0f
#define	LIGHTVIEW_FAR		100.0f

#define PI					(3.14159265f)


#define CHECK_HRESULT(hr)		if (FAILED(hr)) return false
#define CHECK_RESULT(result)	if (!(result)) return false
#define CHECK_RESULT_MSG(result, msg) \
	if (!(result)) { Log::GetInstance()->LogMsg(msg); return false;}
#define CHECK_POINT(ptr)		if (ptr == NULL) return false

//	to release d3dx11 resource
#define SAFE_RELEASE(ptr)\
	if (ptr){\
		(ptr)->Release();\
		(ptr) = NULL;\
	}

#define CLAMP(x,_min,_max)\
	if (x < _min) {x = _min;}\
	if (x > _max) {x = _max;}


#define SAFE_DELETE(ptr) \
if (ptr) {\
	delete (ptr);\
	(ptr) = NULL;\
}

#define SHUTDOWN_DELETE(ptr)\
if (ptr) {\
	ptr->Shutdown();\
	delete ptr;\
	ptr = NULL;\
}

#define SHUTDOWN_RTEXTURE(pTexture)	SHUTDOWN_DELETE(pTexture)
#define SHUTDOWN_MODEL(pModel)		SHUTDOWN_DELETE(pModel)
#define	SHUTDOWN_SHADER(pShader)	SHUTDOWN_DELETE(pShader)


#define KEYS_NUM 31
struct KeysPressed {
	union {
		struct {
			bool w, a, s, d;
			bool o;
			bool left, right, up, down;
			bool pageUp, pageDown;
			
			bool esc;
			bool space;
			bool num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9;
			bool home, end;
			bool tab;
			bool F1, F2, F3, F4;
			bool l_shifit;
		};
		bool _key[KEYS_NUM];
	};
	void Reset() {
		for (size_t i = 0;i < KEYS_NUM; ++i)
			_key[i] = false;
	}
};
extern const UCHAR KeyState[KEYS_NUM];

enum ShaderType {
	DepthShader,NVShader,RTW_DepthShader, Normal, ShadowDepth
};
enum TextureType {
	ScreenTexture, ShadowTexture, WarpMapTexture, ScreenColorTexture
};

extern UINT	G_Flag;