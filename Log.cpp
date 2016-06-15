#include "Log.h"



Log::Log() {
	m_Log.open(".//Error_Log.txt");
	m_Log << "------------- Start Log -------------" << std::endl;
}

Log::~Log() {
	m_Log << "------------- End Log -------------" << std::endl;
	m_Log.close();
}

void Log::LogMsg(const std::string & msg) {
	m_Log << msg << std::endl;
}

void Log::LogMsgInt(int i) {
	m_Log << i << std::endl;
}

void Log::LogShaderErr(ID3D10Blob* errorMsg) {
	if (errorMsg) {
		char * errors = (char*)errorMsg->GetBufferPointer();
		for (size_t i = 0;i < errorMsg->GetBufferSize(); ++i) {
			m_Log << errors[i];
		}
		errorMsg->Release();
		errorMsg = 0;
	}
}

void Log::LogMsgMatrix(const D3DXMATRIX & matr) {
	for (size_t i = 0;i < 4; i++) {
		for (size_t j = 0;j < 4;j++) {
			m_Log << matr.m[i][j] << ' ';
		}
		m_Log << std::endl;
	}
}

void Log::LogVec3(const D3DXVECTOR3 & vec3, std::string name) {
	if (name.size() != 0) {
		m_Log << name << ":\t";
	}
	m_Log << vec3.x << ", " << vec3.y << ", " << vec3.z << std::endl;
}

Log * Log::GetInstance() {
	static Log log;
	return &log;
}
