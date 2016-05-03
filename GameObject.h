
#ifndef GameObject_H
#define GameObject_H

//#include "d3dUtil.h"
#include "d3dUtil.h"
#include "Box.h"
#include "constants.h"
#include "ObstacleObject.h"
#include "d3dApp.h"

class GameObject
{
public:

	GameObject();
	~GameObject();

	virtual char type(){return typeOfCar;};

	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s);
	void draw();
	void update(float dt);

	float getRotY(){return rotY;}
	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {currentForwardSpeed = s;}
	float getSpeed() {return currentForwardSpeed;}
	void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scale = s; radiusSquared = (s*radius)*(s*radius);}
	float getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	bool collided(GameObject *gameObject);
	bool collided(ObstacleObject *obstacleObject);
	void setRotationX(float r) {rotX = r;}
	void setRotationY(float r) {rotY = r;}
	void setRotationZ(float r) {rotZ = r;}
	void setAlreadyCollided(bool ac) {alreadyCollided = ac;}
	bool getAlreadyCollided() {return alreadyCollided;}
	void setHasBoost(bool hasBoost){this->hasBoost = hasBoost;};
	bool getHasBoost() {return hasBoost;}
	void setCObjType(char x) {cObjType = x;}
	char getObjType() {return objType;}

	//define setWorldMatrix here
	void setWorldMatrix(Matrix w) {world = w;}

protected:
	Box *box; 
	bool alreadyCollided;
	Vector3 position;
	char objType;
	bool isJumping;
	bool startOfJump;
	Vector3 velocity;
	float currentPlayerAcceleration;
	float currentForwardSpeed;
	float currentUpwardSpeed;
	float radius;
	float radiusSquared;
	bool active;
	Matrix world;
	float scale;
	ID3D10EffectTechnique* mTech;
	float rotX, rotY, rotZ;
	char typeOfCar;
	bool isBoosting;
	float currentBoostTime;
	bool hasBoost;
	char cObjType;
};


#endif
