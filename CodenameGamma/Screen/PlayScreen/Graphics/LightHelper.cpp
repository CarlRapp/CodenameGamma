#include "LightHelper.h"

XMMATRIX SpotLight::GetViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&Position);
	XMVECTOR dir = XMLoadFloat3(&Direction);

	XMVECTOR target = pos + dir;

	//XMVECTOR up = XMLoadFloat3(XMFLOAT3(
}

XMMATRIX SpotLight::GetProjectionMatrix(float nearZ, float farZ)
{
	XMMATRIX projcetion;
	float fovY = PI * angle / 90.0f;
	float n = 1.0f;
	float f = Range;

	if (!(nearZ < 1.0f || farZ > Range || nearZ > farZ))
	{
		n = nearZ;
		f = farZ;
	}

	return XMMatrixPerspectiveFovLH(fovY, 1, n, f);
}