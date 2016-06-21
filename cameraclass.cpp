#include "cameraclass.h"

Camera::Camera()
{
	m_rotationX = -PI/6;
	m_rotationY = PI/2;

	m_rotationZ = 0.0f;
	m_lookAt = D3DXVECTOR3(0.0f, -0.0f, 1.0f);
	m_position = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
	m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXVec3Cross(&m_right, &m_lookAt, &m_up);

	m_frameTime = 0.0f;

	float fieldOfView = (float)D3DX_PI / 4.0f;
	float screenAspect = (float)SCREEN_WIDTH/ (float)SCREEN_HEIGHT;
	
	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_FAR);
}


Camera::Camera(const Camera& other) {
}


Camera::~Camera() {
}

void Camera::SetFrameTime(float frameTime) {
	m_frameTime = frameTime;
}

void Camera::SetLookAt(float x, float y, float z) {

	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
	D3DXVec3Normalize(&m_lookAt, &m_lookAt);
	m_rotationY = atanf(m_lookAt.z / m_lookAt.x);
	m_rotationX = asinf(m_lookAt.y);
}

void Camera::SetPosition(float x, float y, float z) {
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void Camera::Update(const KeysPressed* keysPressed) {
	D3DXVECTOR3 move(0.0f,0.0f,0.0f);

	if (keysPressed->a)
		move.x -= 1.0f;
	if (keysPressed->d)
		move.x += 1.0f;
	if (keysPressed->w)
		move.z += 1.0f;
	if (keysPressed->s)
		move.z -= 1.0f;
	if (keysPressed->pageUp)
		move.y += 1.0f;
	if (keysPressed->pageDown)
		move.y -= 1.0f;

	D3DXVec3Normalize(&move,&move);

	float moveInterval = moveSpeed * m_frameTime;
	float rotateInterval = turnSpeed * m_frameTime;

	if (keysPressed->left)
		m_rotationY += rotateInterval;
	if (keysPressed->right)
		m_rotationY -= rotateInterval;
	if (keysPressed->up)
		m_rotationX +=  rotateInterval;
	if (keysPressed->down)
		m_rotationX -= rotateInterval;
	
	CLAMP(m_rotationX,-PI/180 * 80 , PI/180 * 80 );
	
	//float x = move.x * -cosf(m_rotationY) - move.z * sinf(m_rotationY);
	//float z = move.x * sinf(m_rotationY) - move.z * cosf(m_rotationY);
	//float y = move.y;

	float x = move.x * sinf(m_rotationY) + move.z * cosf(m_rotationY);
	float y = move.y;
	float z = move.z * sinf(m_rotationY) - move.x * cosf(m_rotationY);

	m_position.x += x * moveInterval;
	m_position.z += z * moveInterval;
	m_position.y += y * moveInterval;


	m_lookAt.x = cosf(m_rotationX) * cosf(m_rotationY);
	m_lookAt.y = sinf(m_rotationX);
	m_lookAt.z = cosf(m_rotationX) * sinf(m_rotationY);
	
}




void Camera::Render(const KeysPressed* keysPressed)
{
	Update(keysPressed);
	D3DXVECTOR3 look = m_lookAt + m_position;
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &look, &m_up);
	
}
void Camera::Render() {
	D3DXVECTOR3 look = m_lookAt + m_position;
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &look, &m_up);
}

void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix) {
	viewMatrix = m_viewMatrix;
}

void Camera::GetProjectionMatrix(D3DXMATRIX& proMatrix) {
	proMatrix = m_projectionMatrix;
}

D3DXVECTOR3 Camera::GetPosition() {
	return m_position;
}

D3DXVECTOR3 Camera::GetLookAt() {
	return m_lookAt;
}

void Camera::GetPosition(D3DXVECTOR3* pos) {
	*pos = m_position;
}
