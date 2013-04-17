#include "Camera.h"


Camera::Camera(void)
{
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	m_Fovy			= (float)PI * 0.45f;
	m_AspectRatio	= 16.0f / 9.0f;
	m_NearZ			= 1.0f;
	m_FarZ			= 5000.0f;	

	m_Position	= XMFLOAT3(0, 0, 0);
	m_Forward	= XMFLOAT3(0, 0, 1);
	m_Right		= XMFLOAT3(1, 0, 0);
	m_Up		= XMFLOAT3(0, 1, 0);

	UpdateView();
	UpdateProjection();
}

Camera::Camera(float fovy, float aspectRatio, float nearZ, float farZ)
{
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	m_Fovy			= fovy;
	m_AspectRatio	= aspectRatio;
	m_NearZ			= nearZ;
	m_FarZ			= farZ;	

	m_Position	= XMFLOAT3(0, 0, 0);
	m_Forward	= XMFLOAT3(0, 0, 1);
	m_Right		= XMFLOAT3(1, 0, 0);
	m_Up		= XMFLOAT3(0, 1, 0);

	UpdateView();
	UpdateProjection();
}


Camera::~Camera(void)
{
}

void Camera::UpdateView()
{
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Forward);
	XMVECTOR P = XMLoadFloat3(&m_Position);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L); 

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));
	
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Forward, L);

	m_View(0,0) = m_Right.x; 
	m_View(1,0) = m_Right.y; 
	m_View(2,0) = m_Right.z; 
	m_View(3,0) = x;   

	m_View(0,1) = m_Up.x;
	m_View(1,1) = m_Up.y;
	m_View(2,1) = m_Up.z;
	m_View(3,1) = y;  

	m_View(0,2) = m_Forward.x; 
	m_View(1,2) = m_Forward.y; 
	m_View(2,2) = m_Forward.z; 
	m_View(3,2) = z;   

	m_View(0,3) = 0.0f;
	m_View(1,3) = 0.0f;
	m_View(2,3) = 0.0f;
	m_View(3,3) = 1.0f;

	/*
	float pr = m_Position.x * m_Right.x		+ m_Position.y * m_Right.y		+ m_Position.z * m_Right.z;
	float pu = m_Position.x * m_Up.x		+ m_Position.y * m_Up.y			+ m_Position.z * m_Up.z;
	float pf = m_Position.x * m_Forward.x	+ m_Position.y * m_Forward.y	+ m_Position.z * m_Forward.z;
	
	m_View = XMFLOAT4X4(  m_Right.x			, m_Up.x				, m_Forward.x			, 0
						, m_Right.y			, m_Up.y				, m_Forward.y			, 0
						, m_Right.z			, m_Up.z				, m_Forward.z			, 0
						, -pr				, -pu					, -pf					, 1);	
	*/
	UpdateFrustum();
}

void Camera::UpdateProjection()
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(m_Fovy, m_AspectRatio, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Projection, proj);
	UpdateFrustum();
}

void Camera::UpdateFrustum()
{
	XMMATRIX proj		= XMLoadFloat4x4(&m_Projection);
	XMMATRIX view		= XMLoadFloat4x4(&m_View);

	XMVECTOR det		= XMMatrixDeterminant(view);
	XMMATRIX invView	= XMMatrixInverse(&det, view);

	m_BoundingFrustum = BoundingFrustum(proj);
	m_BoundingFrustum.Transform(m_BoundingFrustum, invView);
}

void Camera::Pitch(float angle)
{
	XMMATRIX R	= XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

	XMStoreFloat3(&m_Up,		XMVector3TransformNormal(XMLoadFloat3(&m_Up),		R));
	XMStoreFloat3(&m_Forward,	XMVector3TransformNormal(XMLoadFloat3(&m_Forward),	R));
	UpdateView();
}

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right,		XMVector3TransformNormal(XMLoadFloat3(&m_Right),	R));
	XMStoreFloat3(&m_Up,		XMVector3TransformNormal(XMLoadFloat3(&m_Up),		R));
	XMStoreFloat3(&m_Forward,	XMVector3TransformNormal(XMLoadFloat3(&m_Forward),	R));
	UpdateView();
}

void Camera::Rotate(float pitch, float rotY)
{
	XMMATRIX R	= XMMatrixRotationAxis(XMLoadFloat3(&m_Right), pitch);

	XMStoreFloat3(&m_Up,		XMVector3TransformNormal(XMLoadFloat3(&m_Up),		R));
	XMStoreFloat3(&m_Forward,	XMVector3TransformNormal(XMLoadFloat3(&m_Forward),	R));
	UpdateView();

	R = XMMatrixRotationY(rotY);

	XMStoreFloat3(&m_Right,		XMVector3TransformNormal(XMLoadFloat3(&m_Right),	R));
	XMStoreFloat3(&m_Up,		XMVector3TransformNormal(XMLoadFloat3(&m_Up),		R));
	XMStoreFloat3(&m_Forward,	XMVector3TransformNormal(XMLoadFloat3(&m_Forward),	R));

	UpdateView();
}

void Camera::SetForward(XMFLOAT3 forward)
{
	XMVECTOR pos = XMLoadFloat3(&m_Position);
	XMVECTOR direction = XMLoadFloat3(&forward);
	XMVECTOR up	= XMVectorSet(0, 1 , 0, 0);

	if (XMVector3Equal(direction, XMVectorZero()))
		direction = XMVectorSet(0, 0, 1, 0);

	else if (XMVector3Equal(direction, XMVectorSet(0, -1, 0, 0)))
		up = XMVectorSet(0, 0 , 1, 0);

	XMMATRIX view = XMMatrixLookToLH(pos, direction, up);
	
	XMStoreFloat4x4(&m_View, view);

	m_Right		= XMFLOAT3(m_View._11, m_View._21, m_View._31);
	m_Up		= XMFLOAT3(m_View._12, m_View._22, m_View._32);
	m_Forward	= XMFLOAT3(m_View._13, m_View._23, m_View._33);

	UpdateView();
}
