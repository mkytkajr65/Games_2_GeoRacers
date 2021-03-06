
#ifndef ROAD_H
#define ROAD_H

#include "d3dUtil.h"
#include "constants.h"
class Road
{
public:

	Road();
	~Road();

	void init(ID3D10Device* device, float scale);
	void init(ID3D10Device* device, float scale, D3DXCOLOR c);
	void draw();
	void setWorld(Matrix w) {world = w;}
	Matrix getWorld() {return world;}
	void update(float dt);
	void setPosition(Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	Vector3 getNormal() {return norm;}

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	D3DXVECTOR3 norm;

	float rotX, rotY, rotZ;
	Matrix world;
	Vector3 position;
};


#endif 
