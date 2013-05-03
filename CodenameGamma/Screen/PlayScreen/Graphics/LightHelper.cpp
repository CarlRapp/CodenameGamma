#include "LightHelper.h"

/*
std::vector<XMFLOAT4X4> PointLight::GetViewMatrix()
{

}
*/

XMMATRIX PointLight::GetProjectionMatrix(float nearZ, float farZ)
{
	XMMATRIX projcetion;
	float fovY = PI;
	float n = 1.0f;
	float f = Range;

	if (!(nearZ < 1.0f || farZ > Range || nearZ > farZ))
	{
		n = nearZ;
		f = farZ;
	}

	return XMMatrixPerspectiveFovLH(fovY, 1, n, f);
}



XMMATRIX SpotLight::GetViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&Position);
	XMVECTOR dir = XMLoadFloat3(&Direction);	
	dir = XMVector3Normalize(dir);
	XMVECTOR up	= XMVectorSet(0, 1 , 0, 0);

	if (XMVector3Equal(dir, XMVectorZero())) //DETTA SKA ALDRIG SKE!
		dir = XMVectorSet(0, 0, 1, 0);

	else if (XMVector3Equal(dir, XMVectorSet(0, -1, 0, 0)))
		up = XMVectorSet(0, 0 , 1, 0);

	else if (XMVector3Equal(dir, XMVectorSet(0, 1, 0, 0)))
		up = XMVectorSet(0, 0 , -1, 0);

	return XMMatrixLookToLH(pos, dir, up);
}

XMMATRIX SpotLight::GetProjectionMatrix(float nearZ, float farZ)
{
	XMMATRIX projcetion;
	float fovY = 2 * angle;
	float n = 1.0f;
	float f = Range;

	if (!(nearZ < 1.0f || farZ > Range || nearZ > farZ))
	{
		n = nearZ;
		f = farZ;
	}

	return XMMatrixPerspectiveFovLH(fovY, 1, n, f);
}