#include "RoadObject.h"

RoadObject::RoadObject()
{
	speed = 0;
	active = true;
	Identity(&world);
	rotX = 0;
	rotY = 0;
	rotZ = 0;
}

RoadObject::~RoadObject()
{
	road = NULL;
}

void RoadObject::draw()
{
	if (!active)
		return;
    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        road->draw();
    }

}

void RoadObject::init(Road *r, Vector3 pos, float s)
{
	road = r;
	position = pos;
	scale = s;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
}

void RoadObject::update(float dt)
{

	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;
}

