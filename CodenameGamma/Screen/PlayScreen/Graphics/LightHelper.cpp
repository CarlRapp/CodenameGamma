#include "LightHelper.h"


XMMATRIX DirectionalLight::GetViewMatrix(BoundingFrustum& frustum, float offset)
{
	BoundingSphere sphere;
	sphere.CreateFromFrustum(sphere, frustum);

	XMVECTOR center = XMLoadFloat3(&sphere.Center);
	XMVECTOR dir	= XMLoadFloat3(&XMFLOAT3(Direction.x, Direction.y, Direction.z));
	dir = XMVector3Normalize(dir);

	offset += sphere.Radius;
	//XMVECTOR pos	= center - (offset * dir);
	XMVECTOR pos	= center - (2 * offset * dir);


	XMVECTOR up	= XMVectorSet(0, 1 , 0, 0);

	XMFLOAT3 temp;

	XMStoreFloat3(&temp, center);

	if (XMVector3Equal(dir, XMVectorZero())) //DETTA SKA ALDRIG SKE!
		dir = XMVectorSet(0, 0, 1, 0);

	else if (XMVector3Equal(dir, XMVectorSet(0, -1, 0, 0)))
		up = XMVectorSet(0, 0 , 1, 0);

	else if (XMVector3Equal(dir, XMVectorSet(0, 1, 0, 0)))
		up = XMVectorSet(0, 0 , -1, 0);

	return XMMatrixLookAtLH(pos, center, up);
}


XMMATRIX DirectionalLight::GetProjectionMatrix(/*float nearZ, float farZ*/)
{

	float n = 1.0f;
	float f = DIRECTION_MAXRANGE;
	/*
	if (!(nearZ < 1.0f || farZ > DIRECTION_MAXRANGE || nearZ > farZ))
	{
		n = nearZ;
		f = farZ;
	}
	*/
	//XMMatrixOrthographicOffCenterLH(
	return XMMatrixOrthographicLH(1000, 1000, n, f);
}

void DirectionalLight::GetViewProjOBB(BoundingFrustum& frustum, float offset, XMFLOAT4X4& View, XMFLOAT4X4& Proj, BoundingOrientedBox& OBB)
{

	XMFLOAT3 cornersW[frustum.CORNER_COUNT];
	frustum.GetCorners(&cornersW[0]);

	XMVECTOR centerW = XMLoadFloat3(&XMFLOAT3(0,0,0));
	for each (XMFLOAT3 corner in cornersW)
	{
		XMVECTOR temp = XMLoadFloat3(&corner);
		centerW += temp;
	}
	centerW /= 8;
	
	XMVECTOR lightDirW	= XMLoadFloat3(&XMFLOAT3(Direction.x, Direction.y, Direction.z));
	lightDirW = XMVector3Normalize(lightDirW);

	XMVECTOR lightPosW = centerW - (frustum.Far * lightDirW);

	XMVECTOR upW	= XMVectorSet(0, 1 , 0, 0);
	if (XMVector3Equal(lightDirW, XMVectorSet(0, -1, 0, 0)))
		upW = XMVectorSet(0, 0 , 1, 0);

	else if (XMVector3Equal(lightDirW, XMVectorSet(0, 1, 0, 0)))
		upW = XMVectorSet(0, 0 , -1, 0);

	XMMATRIX V = XMMatrixLookAtLH(lightPosW, centerW, upW);

	XMFLOAT3 minV = XMFLOAT3(10000, 10000, 10000);
	XMFLOAT3 maxV = XMFLOAT3(-10000, -10000, -10000);

	for each (XMFLOAT3 corner in cornersW)
	{
		XMVECTOR temp = XMLoadFloat3(&corner);

		XMFLOAT3 cornerV;
		XMStoreFloat3(&cornerV, XMVector3TransformCoord(temp, V));

		minV.x = min(cornerV.x, minV.x);
		minV.y = min(cornerV.y, minV.y);
		minV.z = min(cornerV.z - offset, minV.z);

		maxV.x = max(cornerV.x, maxV.x);
		maxV.y = max(cornerV.y, maxV.y);
		maxV.z = max(cornerV.z, maxV.z);
	}

	// Ortho frustum in light space encloses scene.
	float l = minV.x;
	float b = minV.y;
	float n = minV.z;

	float r = maxV.x;
	float t = maxV.y;
	float f = maxV.z;

	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	
	XMStoreFloat4x4(&View, V);
	XMStoreFloat4x4(&Proj, P);


	std::vector<XMFLOAT3> cornersV;
	cornersV.push_back(minV);
	cornersV.push_back(maxV);

	BoundingBox AABB;
	BoundingBox::CreateFromPoints(AABB, cornersV.size(), &cornersV[0], sizeof(XMFLOAT3));

	BoundingOrientedBox::CreateFromBoundingBox(OBB, AABB);

	XMVECTOR det	= XMMatrixDeterminant(V);
	XMMATRIX iV		= XMMatrixInverse(&det, V);

	OBB.Transform(OBB, iV);
}

XMMATRIX CalculateViewMatrix(XMFLOAT3& Position, XMFLOAT3& Direction)
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

	//XMFLOAT4X4 result;

	//XMStoreFloat4x4(&result, XMMatrixLookToLH(pos, dir, up));

	return XMMatrixLookToLH(pos, dir, up);
}

std::vector<XMFLOAT4X4> PointLight::GetViewMatrixes()
{
	std::vector<XMFLOAT4X4> views;

	XMFLOAT4X4 view;
	XMMATRIX temp;

	//höger
	temp = CalculateViewMatrix(Position, XMFLOAT3(1,0,0));
	XMStoreFloat4x4(&view, temp);
	views.push_back(view);

	//upp
	temp = CalculateViewMatrix(Position, XMFLOAT3(0,1,0));
	XMStoreFloat4x4(&view, temp);
	views.push_back(view);

	//fram
	temp = CalculateViewMatrix(Position, XMFLOAT3(0,0,1));
	XMStoreFloat4x4(&view, temp);
	views.push_back(view);

	//vänster
	temp = CalculateViewMatrix(Position, XMFLOAT3(-1,0,0));
	XMStoreFloat4x4(&view, temp);
	views.push_back(view);

	//ner
	temp = CalculateViewMatrix(Position, XMFLOAT3(0,-1,0));
	XMStoreFloat4x4(&view, temp);
	views.push_back(view);

	//bak
	temp = CalculateViewMatrix(Position, XMFLOAT3(0,0,-1));
	XMStoreFloat4x4(&view, temp);
	views.push_back(view);

	return views;
}

XMMATRIX PointLight::GetProjectionMatrix(/*float nearZ, float farZ*/)
{
	XMMATRIX projcetion;
	float fovY = PI * 0.505f;
	float n = 0.05f;
	float f = Range;
/*
	if (!(nearZ < 1.0f || farZ > Range || nearZ > farZ))
	{
		n = nearZ;
		f = farZ;
	}
	*/
	return XMMatrixPerspectiveFovLH(fovY, 1, n, f);
}

XMMATRIX SpotLight::GetViewMatrix()
{
	return CalculateViewMatrix(Position, Direction);
	/*
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
	*/
}

XMMATRIX SpotLight::GetProjectionMatrix(/*float nearZ, float farZ*/)
{
	XMMATRIX projcetion;
	float fovY = 2 * angle;
	float n = 0.05f;
	float f = Range;
	/*
	if (!(nearZ < 1.0f || farZ > Range || nearZ > farZ))
	{
		n = nearZ;
		f = farZ;
	}
	*/
	return XMMatrixPerspectiveFovLH(fovY, 1, n, f);
}