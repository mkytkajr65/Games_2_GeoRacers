
#include "ObstacleObject.h"

ObstacleObject::ObstacleObject()
{
	radius = 0;
	speed = 10;
	active = true;
	Identity(&world);
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	objType = 'O';
}

ObstacleObject::~ObstacleObject()
{
	ob = NULL;
}

void ObstacleObject::draw()
{
	if (!active)
		return;
    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        ob->draw();
    }
		/*box->draw();*/
}

void ObstacleObject::init(Obstacle *o, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
	ob = o;
	radius = r;
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = r*r;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
}

void ObstacleObject::update(float dt)
{
	position += velocity*dt;
	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;

}

bool ObstacleObject::collided(ObstacleObject *obstacleObject)
{
	Vector3 diff = position - obstacleObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + obstacleObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}