#include "Camera.h"

Camera::Camera()
{
	speed = 10;
	FoV = 0.25*PI;
	aspectRatio = 480.0f/640.0f;
	nearClippingPlane = 1.0f;
	farClippingPlane = 1000.0f;
	up = Vector3(0.0f, 1.0f, 0.0f);
	position = Vector3(-10,0,0);
	lookAt = Vector3(0.0f, 0.0f, 0.0f);
	yaw = 0;
	roll = 0;
	pitch = 0;
}

Camera::~Camera()
{
	//nothing to deallocate
}

void Camera::init(Vector3 p, Vector3 dir, Vector3 _lookAt)
{
	position = p;
	direction = dir;
	lookAt = _lookAt;
}

void Camera::setPerspective()
{
	D3DXMatrixPerspectiveFovLH(&mProj, FoV, aspectRatio, nearClippingPlane,farClippingPlane); 
}
void Camera::update(float dt)
{
	bool yawUpdate = false;
	float deltaYaw = 0;
	float _speed = 20;
	float deltaPitch = 0;

	Vector3 direction = Vector3(0,0,0);
	Matrix yawR;
	Matrix pitchR;
	Matrix rollR;
	
	Identity(&yawR);
	Identity(&pitchR);
	Identity(&rollR);

	if(GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		deltaYaw += _speed*dt;
		yaw += deltaYaw;
		yawUpdate = true;
		//rightDownLastFrame = true;
		//_RPT1(0,"yaw inc %f", deltaYaw);
		//_RPT1(0, "Yaw %f \n", yaw);
	}
	if(GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		yawUpdate = true;
		deltaYaw -= _speed*dt;
		yaw+= deltaYaw;
		//leftDownLastFrame = true;
		//_RPT1(0,"yaw dec %f ", deltaYaw);
		//_RPT1(0, "Yaw %f \n", yaw);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		yawUpdate = true;
		deltaPitch += _speed*dt;
		if (deltaPitch > 1) 
			deltaPitch = 1;
		pitch += deltaPitch;
		//leftDownLastFrame = true;
		_RPT1(0,"deltaPitch dec %f ", deltaPitch);
		_RPT1(0, "Pitch %f \n", pitch);
	}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		yawUpdate = true;
		deltaPitch -= _speed*dt;
		if (deltaPitch < -1) 
			deltaPitch = -1;
		pitch += deltaPitch;
		//leftDownLastFrame = true;
		_RPT1(0,"deltaPitch dec %f ", deltaPitch);
		_RPT1(0, "Pitch %f \n", pitch);
	}
	/*RotateY(&yawR, ToRadian( yaw));
	RotateZ(&pitchR, ToRadian(pitch));*/


	if(GetAsyncKeyState('A') & 0x8000)
			direction.z = -1;
	if(GetAsyncKeyState('D') & 0x8000)
			direction.z = 1;
	if(GetAsyncKeyState('S') & 0x8000)
			direction.x = 1;
	if(GetAsyncKeyState('W') & 0x8000)
			direction.x = -1;


	D3DXVec3Normalize(&direction, &direction);
	Matrix temp = yawR;
	Transform(&direction, &direction, &yawR);
	Vector3 foo = direction;
	direction = direction*_speed*dt;
	position += direction;

	if (yawUpdate)
	{
	Vector3 transformedRef = Vector3(1,0,0);   
	Transform(&transformedRef, &transformedRef,&temp); //
	D3DXVec3Normalize(&transformedRef, &transformedRef);
    lookAt = transformedRef * 10;
	lookAt += position;
	//_RPT1(0,"lookAt x %f ", lookAt.x);
	//_RPT1(0,"  lookAt z %f\n",lookAt.z);

	yawUpdate = false;
	}
	else{
		lookAt += direction;
	//_RPT1(0,"lookAt x %f ", lookAt.x);
	//_RPT1(0,"  lookAt z %f\n",lookAt.z);
	}
	D3DXMatrixLookAtLH(&mView, &position, &lookAt, &up);
}
