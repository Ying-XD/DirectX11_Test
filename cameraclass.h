#pragma once

#include <d3dx10math.h>
#include"global.h"

class Camera {
private:
	const FLOAT moveSpeed = 0.005f;
	const FLOAT turnSpeed = 0.0015f;
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetFrameTime(float frameTime);
	void SetLookAt(float x,float y,float z);
	void SetPosition(float x,float y,float z);


	void Render(const KeysPressed* keysPressed);
	void Render();
	void GetViewMatrix(D3DXMATRIX& viewMatrix);
	void GetProjectionMatrix(D3DXMATRIX& proMatrix);
	
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetLookAt();
	void GetPosition(D3DXVECTOR3* pos);
private:
	void Update(const KeysPressed* keysPressed);

	float m_rotationX, m_rotationY, m_rotationZ;
	
	D3DXMATRIX m_viewMatrix;

	D3DXVECTOR3 m_lookAt;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_up;
	D3DXVECTOR3 m_right;
	
	FLOAT m_frameTime;
	D3DXMATRIX m_projectionMatrix;
	
};
