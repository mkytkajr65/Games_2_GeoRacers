
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
	Vector3 getCurrentWayPoint() {return currentWayPoint;}
	void setCurrentWayPoint(Vector3 w) {currentWayPoint = w;}

	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s);

private:
	Vector3 currentWayPoint;
};


#endif
