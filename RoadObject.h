#ifndef RoadObject_H
#define RoadObject_H

#include "d3dUtil.h"

#include "Road.h"
#include "constants.h"

class RoadObject
{
public:

	RoadObject();
	~RoadObject();

	void init(Road *r,  Vector3 pos, float s);
	void draw();
	void update(float dt);

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scale = s;}
	float getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	void setRotationX(float r) {rotX = r;}
	void setRotationY(float r) {rotY = r;}
	void setRotationZ(float r) {rotZ = r;}
	float getRotationY() {return rotY;}
private:
	Road *road; 
	Vector3 position;
	Vector3 velocity;
	float speed;
	bool active;
	Matrix world;
	float scale;
	ID3D10EffectTechnique* mTech;
	float rotX, rotY, rotZ;
};

#endif
