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

void Camera::init(Vector3 p, Vector3 dir, Vector3 _lookAt, GameObject *pl)
{
	//this->direction = direction;
	this->lookAt = _lookAt;
	speed = 10;
	FoV = 0.25*PI;
	aspectRatio = 480.0f/640.0f;
	nearClippingPlane = 1.0f;
	farClippingPlane = 1000.0f;
	up = Vector3(0.0f, 1.0f, 0.0f);
	yaw = 0;
	roll = 0;
	pitch = 0;
	position = p;
	direction = dir;
	player = pl;
	lastRotation = 0.0;
}

void Camera::setPerspective()
{
	D3DXMatrixPerspectiveFovLH(&mProj, FoV, aspectRatio, nearClippingPlane,farClippingPlane); 
}
void Camera::update(float dt)
{
	//_RPT1(0, "Player position %f \n", player->getPosition().z);
	bool yawUpdate = false;
	float deltaYaw = 0;
	float _speed = 20;
	float deltaPitch = 0;

	//Vector3 direction = Vector3(0,0,0);
	Matrix yawR;
	Matrix pitchR;
	Matrix rollR;
	
	Identity(&yawR);
	Identity(&pitchR);
	Identity(&rollR);

	RotateY(&yawR, ToRadian(player->getRotY()));

	//_RPT1(0,"  rotation %f\n",ToRadian(player->getRotY()));

	//starting from the player postion, translate camera by a specified offset

	Matrix offsetTranslate;

	Vector3 offset = Vector3(0, 2, -10);

	//Translate(&offsetTranslate, 0, 2, -10);

	Vector3 transformedReference;

	Transform(&transformedReference, &offset, &yawR);

	//Transform(&transformedReference, &transformedReference, &yawR);

	position = player->getPosition() + transformedReference;

	lookAt = player->getPosition();

	D3DXMatrixLookAtLH(&mView, &position, &lookAt, &up);
}
