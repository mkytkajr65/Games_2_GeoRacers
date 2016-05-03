
#include "GameObject.h"

GameObject::GameObject()
{
	radius = 0;
	currentForwardSpeed = 10;
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
	currentForwardSpeed = sp;
	scale = s;
	radiusSquared = r*r;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	alreadyCollided = false;
	typeOfCar = 'p';
	currentPlayerAcceleration = 0.0f;
	currentUpwardSpeed = 0;
	isJumping = false;
	startOfJump = false;
	hasBoost = true;//to change to false
	isBoosting = false;
	currentBoostTime = 0.0;
}

void GameObject::update(float dt)
{	
	if(isBoosting)
	{
		if(currentBoostTime < MAX_BOOST_TIME)
		{
			currentBoostTime += dt;
		}else{
			isBoosting = false;
			currentForwardSpeed = PLAYER_MAX_VELOCITY;
		}
	}
	

	if(isJumping && position.y <= 0.0f)
	{
		position.y = 0.0f;
		currentUpwardSpeed = 0.0f;
		isJumping = false;
	}

	float frameAcceleration = 0.0f;

	if(GetAsyncKeyState(VK_SPACE) & 0x8000){
		if(!isJumping)
		{
			isJumping = true;
			startOfJump = true;
		}
	}
	if(GetAsyncKeyState('B') & 0x8000){
		if(hasBoost && !isBoosting)
		{
			isBoosting = true;
			hasBoost = false;
		}
	}
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
	if((currentForwardSpeed <= 0.3f && currentForwardSpeed >= -0.3f) && frameAcceleration == 0.0f){
		currentForwardSpeed = 0.0f;
	}
	//if no acceleration keys are pressed and speed is above threshold, then decelerate
	else if(frameAcceleration == 0.0f && currentForwardSpeed > 0.3f){
		frameAcceleration = -0.4f;
	}
	//if no acceleration keys are pressed and speed is below threshold, then accelerate
	else if(frameAcceleration == 0.0f && currentForwardSpeed < -0.3f){
		frameAcceleration = 0.4f;
	}

	currentPlayerAcceleration += frameAcceleration;

	if(currentPlayerAcceleration>PLAYER_MAX_ACCELERATION){
		currentPlayerAcceleration = PLAYER_MAX_ACCELERATION;
	}else if(currentPlayerAcceleration<-PLAYER_MAX_ACCELERATION){
		currentPlayerAcceleration = -PLAYER_MAX_ACCELERATION;
	}

	if(isJumping && startOfJump)
	{
		currentUpwardSpeed = INITIAL_PLAYER_JUMP_VELOCITY;
		startOfJump = false;
	}
	else if(isJumping)
	{
		currentUpwardSpeed -= GRAVITY;
	}

	currentForwardSpeed += currentPlayerAcceleration;
	if(!isBoosting){
		if(currentForwardSpeed > PLAYER_MAX_VELOCITY){
			currentForwardSpeed = PLAYER_MAX_VELOCITY ;
		}else if(currentForwardSpeed < -PLAYER_MAX_VELOCITY){
			currentForwardSpeed = -PLAYER_MAX_VELOCITY ;
		}
	}
	else
	{
		currentForwardSpeed = BOOST_VELOCITY;
	}

	_RPT1(0,"  current speed %f\n",currentForwardSpeed);

	Vector3 temp = Vector3(0,currentUpwardSpeed,currentForwardSpeed);

	Matrix m;
	RotateY(&m, ToRadian(rotY));

	Vector3 effectiveVelocity;

	Transform(&effectiveVelocity, &temp,&m);

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