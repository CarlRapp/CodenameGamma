#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "../../stdafx.h"

class Camera
{
private:

	D3D11_VIEWPORT m_ViewPort;

	float		m_Fovy,		m_AspectRatio,	m_NearZ, m_FarZ;
	XMFLOAT3	m_Position, m_Forward,		m_Right, m_Up;
	XMFLOAT4X4	m_View,		m_Projection;

	BoundingFrustum m_BoundingFrustum;

	void UpdateView();
	void UpdateProjection();
	void UpdateFrustum();

public:
	Camera(void);
	Camera(float fovy, float aspectRatio, float nearZ, float farZ);
	~Camera(void);

	void Rotate(float pitch, float rotY);

	void Pitch(float angle);
	void RotateY(float angle);


#pragma region Get Functions
	XMFLOAT4X4 GetView()		{ return m_View; } 
	XMFLOAT4X4 GetProjection()	{ return m_Projection; } 

	XMFLOAT3 GetPosition()		{ return m_Position; }
	XMFLOAT3 GetForward()		{ return m_Forward; }
	XMFLOAT3 GetRight()			{ return m_Right; }
	XMFLOAT3 GetUp()			{ return m_Up; }

	BoundingFrustum GetFrustum() { return m_BoundingFrustum; }

	D3D11_VIEWPORT GetViewPort() { return m_ViewPort; }

#pragma endregion

#pragma region Set Functions
	void SetFOVY(float fovy)				{ m_Fovy		= fovy;			UpdateProjection(); }
	void SetAspectRatio(float aspectRatio)	{ m_AspectRatio = aspectRatio;	UpdateProjection(); }
	void SetNearZ(float nearZ)				{ m_NearZ		= nearZ;		UpdateProjection(); }
	void SetFarZ(float farZ)				{ m_FarZ		= farZ;			UpdateProjection(); }

	void SetPosition(XMFLOAT3 position)			{ m_Position = position; UpdateView(); }
	void SetPosition(float x, float y, float z)	{ SetPosition(XMFLOAT3(x, y, z)); }
	void SetForward(XMFLOAT3 forward);
	void SetForward(float x, float y, float z)	{ SetForward(XMFLOAT3(x, y, z)); }
	
	void SetLookAt(XMFLOAT3 target)				
	{ 
		XMVECTOR targ	= XMLoadFloat3(&target);
		XMVECTOR pos	= XMLoadFloat3(&m_Position);
		XMFLOAT3 forward;
		XMStoreFloat3(&forward, targ - pos);
		SetForward(forward);
	}
	
	void SetLookAt(float x, float y, float z)	{ SetLookAt(XMFLOAT3(x, y, z)); }

	void SetViewPort(int X, int Y, int Width, int Height)
	{
		m_ViewPort.TopLeftX = (float)X;
		m_ViewPort.TopLeftY = (float)Y;
		m_ViewPort.Width	= (float)Width;
		m_ViewPort.Height	= (float)Height;
	}
#pragma endregion

};

#endif
