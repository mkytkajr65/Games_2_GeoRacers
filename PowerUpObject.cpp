
#include "PowerUpObject.h"

PowerUpObject::PowerUpObject()
{
	radius = 0;
	speed = 10;
	active = true;
	Identity(&world);
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	alreadyCollided = false;
	objType = 'P';
}

PowerUpObject::~PowerUpObject()
{
	box = NULL;
}

bool PowerUpObject::collided(ObstacleObject *obstacleObject)
{
	Vector3 diff = position - obstacleObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + obstacleObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}

void PowerUpObject::draw()
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

void PowerUpObject::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
	box = b;
	radius = r;
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = r*r;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	alreadyCollided = false;
	typeOfCar = 'p';
}

void PowerUpObject::update(float dt)
{
	float velX, velY, velZ;
	Vector3 newVel;

	velX = getVelocity().x;
	velY = 0.0;
	velZ = getVelocity().z + PLAYER_MAX_ACCELERATION;

	

	if(velX > PLAYER_MAX_VELOCITY){
		velX = PLAYER_MAX_VELOCITY ;
	}else if(velX < -PLAYER_MAX_VELOCITY){
		velX = -PLAYER_MAX_VELOCITY ;
	}

	if(velZ > PLAYER_MAX_VELOCITY){
		velZ = PLAYER_MAX_VELOCITY ;
	}else if(velZ < -PLAYER_MAX_VELOCITY){
		velZ = -PLAYER_MAX_VELOCITY ;
	}

	newVel = Vector3(velX, 0, velZ);

	position += newVel*dt;
	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;

}

bool PowerUpObject::collided(GameObject *gameObject)
{
	Vector3 diff = position - gameObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + gameObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}