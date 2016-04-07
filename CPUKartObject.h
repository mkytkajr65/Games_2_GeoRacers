
#ifndef CPUKartObject_H
#define CPUKartObject_H

#include "GameObject.h"
#include "ObstacleObject.h"

class CPUKartObject : public GameObject
{
public:

	CPUKartObject();
	~CPUKartObject();

	void update(float dt);
	bool collided(GameObject *gameObject);
	bool collided(ObstacleObject *obstacleObject);
	char type(){return typeOfCar;};

	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s);
};


#endif
