#pragma once
#include <string>
#include <fstream>
#include <D3D10.h>
#include <D3DX10math.h>
class Log {
public:
	Log();
	~Log();
	void LogMsg() {};
	void LogMsg(const std::string &msg);
	void LogMsgInt(int i);
	void LogShaderErr(ID3D10Blob * errorMsg);
	void LogMsgMatrix(const D3DXMATRIX& matr);
	void LogVec3(const D3DXVECTOR3& vec3, std::string name = "");
	static Log * GetInstance();
private:
	std::ofstream m_Log;
};

