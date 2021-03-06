
#include "CPUKartObject.h"
#include "d3dApp.h"

CPUKartObject::CPUKartObject()
{
	radius = 0;
	currentForwardSpeed = 10;
	active = true;
	Identity(&world);
	rotX = 0;
	rotY = 0;
	rotZ = 0;
}

void CPUKartObject::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
	box = b;
	radius = r;
	position = pos;
	velocity = vel;
	currentForwardSpeed = vel.z;
	scale = s;
	radiusSquared = r*r;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	alreadyCollided = false;
	typeOfCar = 'c';
	currentWayPoint = Vector3(0, 0, 0);
}

CPUKartObject::~CPUKartObject()
{
	box = NULL;
}

void CPUKartObject::update(float dt)
{
	//_RPT1(0, "Rand Velocity %f \n", velocity.z);
	Vector3 direction = currentWayPoint - position;
	D3DXVec3Normalize(&direction, &direction);
	velocity = Vector3(currentForwardSpeed*direction.x, 0, currentForwardSpeed*direction.z);

	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ);
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;
	position += velocity*dt;
}

bool CPUKartObject::collided(GameObject *gameObject)
{
	Vector3 diff = position - gameObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + gameObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}

bool CPUKartObject::collided(ObstacleObject *obstacleObject)
{
	Vector3 diff = position - obstacleObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + obstacleObject->getRadiusSquare();
	if (length <= radii) {
		velocity = velocity/2;
		return true;
	}
	return false;
}