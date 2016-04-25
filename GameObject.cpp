
#include "GameObject.h"

GameObject::GameObject()
{
	radius = 0;
	currentSpeed = 10;
	active = true;
	Identity(&world);
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	alreadyCollided = false;
}

GameObject::~GameObject()
{
	box = NULL;
}

bool GameObject::collided(ObstacleObject *obstacleObject)
{
	Vector3 diff = position - obstacleObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + obstacleObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}

void GameObject::draw()
{
	if (!active)
		return;
    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        box->draw();
    }
		/*box->draw();*/
}

void GameObject::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
	box = b;
	radius = r;
	position = pos;
	velocity = vel;
	currentSpeed = sp;
	scale = s;
	radiusSquared = r*r;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	alreadyCollided = false;
	typeOfCar = 'p';
	currentPlayerAcceleration = 0.0f;
}

void GameObject::update(float dt)
{	

	//float acceleration = -0.13;

	float frameAcceleration = 0.0f;

	if(GetAsyncKeyState('A') & 0x8000){
		rotY -= 0.02;
	}
	if(GetAsyncKeyState('D') & 0x8000){
		rotY += 0.02;
	}
	if(GetAsyncKeyState('W') & 0x8000){
		frameAcceleration = 1.0;
	}
	if(GetAsyncKeyState('S') & 0x8000){
		frameAcceleration = -1.0;
	}

	//if no acceleration keys are pressed and current speed is close to 0, stop the Kart
	if((currentSpeed <= 0.3f && currentSpeed >= -0.3f) && frameAcceleration == 0.0f){
		currentSpeed = 0.0f;
	}
	//if no acceleration keys are pressed and speed is above threshold, then decelerate
	else if(frameAcceleration == 0.0f && currentSpeed > 0.3f){
		frameAcceleration = -0.4f;
	}
	//if no acceleration keys are pressed and speed is below threshold, then accelerate
	else if(frameAcceleration == 0.0f && currentSpeed < -0.3f){
		frameAcceleration = 0.4f;
	}

	currentPlayerAcceleration += frameAcceleration;

	if(currentPlayerAcceleration>PLAYER_MAX_ACCELERATION){
		currentPlayerAcceleration = PLAYER_MAX_ACCELERATION;
	}else if(currentPlayerAcceleration<-PLAYER_MAX_ACCELERATION){
		currentPlayerAcceleration = -PLAYER_MAX_ACCELERATION;
	}
	
	_RPT1(0,"  current speed %f\n",currentSpeed);

	currentSpeed += currentPlayerAcceleration;
	if(currentSpeed > PLAYER_MAX_VELOCITY){
		currentSpeed = PLAYER_MAX_VELOCITY ;
	}else if(currentSpeed < -PLAYER_MAX_VELOCITY){
		currentSpeed = -PLAYER_MAX_VELOCITY ;
	}

	Vector3 temp = Vector3(0,0,currentSpeed);

	Matrix m;
	RotateY(&m, ToRadian(rotY));

	Vector3 effectiveVelocity;

	Transform(&effectiveVelocity, &temp,&m);

	/*velocity.z += (currentPlayerAcceleration * cos(ToRadian(rotY)));
	velocity.x += (currentPlayerAcceleration * sin(ToRadian(rotY)));*/

	//if((velocity.z < 0 && (degrees > -90.0f && degrees < 90.0f)) || (velocity.z > 0 && (degrees < -90.0f && degrees > 90.0f))   && acceleration==0){ velocity.z = 0; }

	

	if(effectiveVelocity.z > PLAYER_MAX_VELOCITY){
		effectiveVelocity.z = PLAYER_MAX_VELOCITY ;
	}else if(effectiveVelocity.z < -PLAYER_MAX_VELOCITY){
		effectiveVelocity.z = -PLAYER_MAX_VELOCITY ;
	}

	velocity = effectiveVelocity;

	position += effectiveVelocity*dt;
	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, ToRadian(rotY));
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;

}

bool GameObject::collided(GameObject *gameObject)
{
	Vector3 diff = position - gameObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + gameObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}