//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "CPUKartObject.h"
#include "Obstacle.h"
#include "ObstacleObject.h"
#include "Line.h"
#include <d3dx9math.h>
#include "Camera.h"
#include "LineObject.h"
#include "Quad.h"
#include "Road.h"
#include "RoadObject.h"
//#include "KartPlace.h"
#include <sstream>
#include "Light.h"
#include "audio.h"
#include "PowerUpObject.h"
#include "TreeSprites.h"


class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();

	//Audio *audio;

private:
	// Camera stuff
	Vector3 cameraPos;
	Vector3 lookAt;
	Camera camera;

	Light mParallelLight;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;
	ID3D10ShaderResourceView* boxTexVar;
	ID3D10ShaderResourceView* boostTexVar;
	ID3D10ShaderResourceView* carTexVar;
	ID3D10ShaderResourceView* splashTex;

	bool revPlaying;

	GameStates gameStates ;

	int playerPosition;

	Line line;
	//Add line, box, and gameobject definitions here
	Box pKart, cKart, boostBox;
	Obstacle obstacle;
	GameObject playerKart;
	CPUKartObject CPUKarts[CPU_KARTS];
	Quad splash;
	ObstacleObject obstacles[OBSTACLES];
	PowerUpObject boosts[POWER_UPS];
	int laps;
	int cLaps[CPU_KARTS];

	TreeSprites mTrees;
	Road road;
	RoadObject roads[ROADS];
	D3DXVECTOR3 waypoints[ROADS];
	LineObject xLine, yLine, zLine;
	Light mLight;
	Audio *audio;

	bool gameOver;

	float spinAmount;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;


	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;

	ID3D10EffectMatrixVariable* mfxTexMtxVar;

	D3DXMATRIX mGeoWorld;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
	float boostTimer;
	float mTheta;
	float mPhi;
	int w[CPU_KARTS+1];

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);

	theApp.initApp();

	return theApp.run();
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mFX(0), mTech(0), mfxWVPVar(0), mfxWorldVar(0), mfxEyePosVar(0),
	mfxLightVar(0), mfxDiffuseMapVar(0), mfxSpecMapVar(0), mfxTexMtxVar(0), 
	mVertexLayout(0), mDiffuseMapRV(0), mSpecMapRV(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	D3DXMatrixIdentity(&mGeoWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mDiffuseMapRV);
	ReleaseCOM(mSpecMapRV);
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	buildFX();
	buildVertexLayouts();

	revPlaying = false;

	gameStates = gameMenu;
	//mCarMesh.init(md3dDevice, 1.0f);

	laps = 0;

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"../Games_2_GeoRacers/test.png", 0, 0, &mDiffuseMapRV, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"../Games_2_GeoRacers/defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"../Games_2_GeoRacers/box.png", 0, 0, &boxTexVar, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"../Games_2_GeoRacers/boost.png", 0, 0, &boostTexVar, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"../Games_2_GeoRacers/stripe.png", 0, 0, &carTexVar, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"../Games_2_GeoRacers/WoodCrate01.dds", 0, 0, &splashTex, 0 ));
	mParallelLight.dir      = D3DXVECTOR3(0.0, -.707f, -.707f);
	mParallelLight.ambient  = D3DXCOLOR(0.4f, 0.4f, 0.5f, 1.0f);
	mParallelLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	spinAmount = 0;

	gameOver = false;

	playerPosition = -1;

	boostTimer = 0;

	pKart.init(md3dDevice, 1, CHARCOAL_GREY);
	cKart.init(md3dDevice, 1, RED);
	boostBox.init(md3dDevice, 1, GREEN);
	obstacle.init(md3dDevice, 1, WHITE);


	playerKart.init(&pKart, 2, Vector3(0,0,0),Vector3(0,0,0),0,1);

	D3DXCOLOR colors [ROADS] = {YELLOW, GREEN, BLUE};

	splash.init(md3dDevice,50.0f,WHITE);

	float roadZLength = 100.0f;

	float zPos = 0;

	float angle = 0.0f;

	road.init(md3dDevice,1,WHITE);

	for(int i = 0;i<ROADS;i++){
		roads[i].init(&road, Vector3(0,0,0), 1);

		Vector3 temp1 = Vector3(0,0,45);

		Matrix m1;

		Identity(&m1);
		RotateY(&m1, ToRadian(angle));

		Vector3 eV;

		Transform(&eV, &temp1,&m1);


		if(i>0)
		{
			Vector3 newPosition = roads[i-1].getPosition() +  eV;
			roads[i].setPosition(newPosition);
		}
		else
		{
			roads[i].setPosition(Vector3(0, -1.2,0));
		}

		roads[i].setRotationY(ToRadian(angle));

		//zPos += roadZLength;
		angle += 1;
		waypoints[i] = roads[i].getPosition();
	}

	/*for(int i = 0;i<ROADS;i++)
	{
		road[i].init(md3dDevice,1, colors[i]);
		road[i].setPosition(Vector3(0,-1.2,zPos));
		zPos += roadZLength;
	}*/


	//float d = D3DXVec3Dot(&(-mParallelLight.dir), &roads[0].getNormal());

	float randZPos, randXPos;

	float totalRoadZLength = roadZLength * ROADS;

	float roadXLength = 20;

	float horOffset, fbOffset, powOffset;

	Vector3 tempPosition;

	float maxHorOffset = ROAD_WIDTH/2;
	float minHorOffset = -ROAD_WIDTH/2;

	for(int i = 0; i < OBSTACLES; i++) {
			obstacles[i].init(&obstacle,1.0f,Vector3(0,0,0),Vector3(0,0,0),0,1);

			tempPosition = roads[i].getPosition();

			angle = roads[i].getRotationY();

			horOffset = ((int)maxHorOffset - minHorOffset) * ( (double)rand() / (double)RAND_MAX ) + minHorOffset;

			Vector3 temp1 = Vector3(horOffset,0, 0);

			Matrix m1;

			Identity(&m1);
			RotateY(&m1, ToRadian(angle));

			Vector3 eV;

			Transform(&eV, &temp1,&m1);


			if(i>0)
			{
				Vector3 newPosition1 = roads[i-1].getPosition() +  eV;
				obstacles[i].setPosition(newPosition1);
			}
			else
			{
				obstacles[i].setPosition(roads[i].getPosition() + eV);
			}
			obstacles[i].setRotationY(angle);
	}
	
	int interval = (ROADS / POWER_UPS )- 1;

	for(int i = 0; i < POWER_UPS; i++) {
			boosts[i].init(&boostBox,1.0f,Vector3(0,0,0),Vector3(0,0,0),0,1);

			tempPosition = roads[i * interval].getPosition();

			angle = roads[(i*interval)].getRotationY();

			horOffset = ((int)maxHorOffset - minHorOffset) * ( (double)rand() / (double)RAND_MAX ) + minHorOffset;

			Vector3 temp1 = Vector3(horOffset,0, 0);

			Matrix m1;

			Identity(&m1);
			RotateY(&m1, ToRadian(angle));

			Vector3 eV;

			Transform(&eV, &temp1,&m1);

			if(i>0)
			{
				Vector3 newPosition = roads[(i*interval)-1].getPosition() +  eV;
				boosts[i].setPosition(newPosition);
			}
			else
			{
				boosts[i].setPosition(roads[(i*interval)].getPosition() + eV);
			}

			boosts[i].setRotationY(angle);
	}
	//float randVelocity;
	int maxVelocity = PLAYER_MAX_VELOCITY;

	for(int i = 0; i < CPU_KARTS; i++) {
		w[i] = 0;
		cLaps[i] = 0;
		CPUKarts[i].init(&cKart,2,Vector3(0,0,0),Vector3(0,0,0),0,1);
		CPUKarts[i].setCurrentWayPoint(Vector3(waypoints[w[i]].x + ((rand()%40)-20), waypoints[w[i]].y, waypoints[w[i]].z + ((rand()%40)-20)));
		if (i==0) {
			CPUKarts[i].setPosition(Vector3(playerKart.getPosition().x + 1.5, 0,playerKart.getPosition().z + 2));
		}
		else {
			CPUKarts[i].setPosition(Vector3(CPUKarts[i-1].getPosition().x + 1.5, 0,CPUKarts[i-1].getPosition().z + 2));
		}
	}w[CPU_KARTS] = 0;
	playerKart.setCurrentWayPoint(waypoints[w[CPU_KARTS]]);

	//CAMERA initialization here
	/*mLight.dir      = D3DXVECTOR3(0.0f, -0.707f, -0.707f);
	mLight.ambient  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLight.diffuse  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLight.specular = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);*/

	/*mLight2.ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLight2.diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLight2.specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLight2.att.x    = 0.0f;
	mLight2.att.y    = 0.0f;
	mLight2.att.z    = 1.0f;
	mLight2.spotPow  = 64.0f;
	mLight2.range    = 10000.0f;
	mLight2.pos = D3DXVECTOR3(playerKart.getPosition().x + .25, playerKart.getPosition().y, playerKart.getPosition().z);

	mLight3.ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLight3.diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLight3.specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLight3.att.x    = 0.0f;
	mLight3.att.y    = 0.0f;
	mLight3.att.z    = 1.0f;
	mLight3.spotPow  = 64.0f;
	mLight3.range    = 10000.0f;
	mLight3.pos = D3DXVECTOR3(playerKart.getPosition().x + .75, playerKart.getPosition().y, playerKart.getPosition().z);*/

	//camera.init(Vector3(playerKart.getPosition().x + 10,playerKart.getPosition().y + 2,playerKart.getPosition().z), Vector3(0,0,0), Vector3(-1,.2,0));


	D3DXVECTOR3 treeCenters[16];
	for(UINT i = 0; i < 16; ++i)
	{
		float x = RandF(-35.0f, 35.0f);
		float z = RandF(100.0f, 550.0f);
		float y = -1.2;

		// Move tree slightly above land height.
		y += 16.0f;

		treeCenters[i] = D3DXVECTOR3(x,y,z);

	}
	mTrees.init(md3dDevice,treeCenters,16);


	camera.init(Vector3(0,2,-10), Vector3(0,0,0), Vector3(0,0,10), &playerKart);
	camera.setPerspective();

	audio = new Audio();
	if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
	{
		if (!audio->initialize()){

		}
	}

	line.init(md3dDevice, 1.0f, WHITE);
	xLine.init(&line, Vector3(0,0,0), 5);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&line, Vector3(0,0,0), 5);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&line, Vector3(0,0,0), 5);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(270));

	audio->playCue(CIRCUIT);
}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	if(gameStates == gameMenu)
	{
		splash.setPosition(playerKart.getPosition());
		if(GetAsyncKeyState(VK_RETURN) & 0x8000){
			gameStates = gamePlay;
		}
	}
	else if(gameStates == gamePlay)
	{
		D3DApp::updateScene(dt);


		xLine.update(dt);
		yLine.update(dt);
		zLine.update(dt);

		playerKart.update(dt);
		
		camera.update(dt);

		float randOffset, baseVelocity;

		bool kartHit;

		for(int i = 0; i < CPU_KARTS; i++) {
			randOffset = ((int)60 - (-10)) * ( (double)rand() / (double)RAND_MAX ) + (-20);
			kartHit = false;
			for(int j = 0;j<OBSTACLES;j++)
			{
				if(CPUKarts[i].collided(&obstacles[j]))
				{
					baseVelocity = 0.0;
					randOffset = 5.0f;
					kartHit = true;
					break;
				}
			}
			if(!kartHit)
			{
				if(playerKart.getSpeed() > PLAYER_MAX_VELOCITY/2)
				{
					baseVelocity = playerKart.getSpeed();
				}
				else
				{
					baseVelocity = PLAYER_MAX_VELOCITY/3;
				}
			}

			CPUKarts[i].setSpeed(baseVelocity + randOffset);
			CPUKarts[i].update(dt);
		}

		_RPT1(0, "KART SPEED %f \n", CPUKarts[0].getSpeed());
		

		/*for (int i = 0; i < CPU_KARTS; i++) {
		float oldCPUVel = CPUKarts[i].getVelocity().z;
		CPUKarts[i].update(dt);
		for(int j = 0; j < OBSTACLES; j++) {
		if(CPUKarts[i].getPosition().z >= obstacles[j].getPosition().z - 6) {
		if(CPUKarts[i].getPosition().x > obstacles[j].getPosition().x - 3 && CPUKarts[i].getPosition().x <= obstacles[j].getPosition().x + .5) {
		int dodge = rand()%2;
		if(dodge == 1)
		CPUKarts[i].setVelocity(Vector3(-.2, 0, oldCPUVel));
		}
		else if(CPUKarts[i].getPosition().x <= obstacles[j].getPosition().x + 3 && CPUKarts[i].getPosition().x > obstacles[j].getPosition().x + .5) {
		int dodge = rand()%2;
		if(dodge == 1)
		CPUKarts[i].setVelocity(Vector3(.2, 0, oldCPUVel));
		}
		else {
		CPUKarts[i].setVelocity(Vector3(0, 0, oldCPUVel));
		}
		}
		}
		}*/


		for (int i = 0; i < OBSTACLES; i++) {
			obstacles[i].update(dt);
		}
		for (int i = 0; i < POWER_UPS; i++) {
			boosts[i].update(dt);
		}
		for(int i = 0;i<ROADS;i++)
		{
			roads[i].update(dt);
		}

		int count = 0;
		for(int i = 0;i<OBSTACLES;i++)
		{
			if(playerKart.collided(&obstacles[i]) && !playerKart.getAlreadyCollided())
			{
				playerKart.setAlreadyCollided(true);
				playerKart.setCObjType('O');
				playerKart.setVelocity(Vector3(0,0,0));
				audio->playCue(SQUEAL);
			}
			else if(playerKart.collided(&obstacles[i]) && playerKart.getAlreadyCollided()) {
				playerKart.setVelocity(Vector3(0, 0, 5));
			}
			else if(!playerKart.collided(&obstacles[i]) && playerKart.getAlreadyCollided()){
				count++;

				if(count == OBSTACLES){
					playerKart.setCObjType('N');
					playerKart.setAlreadyCollided(false);
				}
			}
		}

		for(int i = 0; i < CPU_KARTS; i++) {
			if(CPUKarts[i].getPosition().z >= waypoints[w[i]].z -15 && CPUKarts[i].getPosition().x >= waypoints[w[i]].x-15 && CPUKarts[i].getPosition().z <= waypoints[w[i]].z +15 && CPUKarts[i].getPosition().x <= waypoints[w[i]].x+15) {
				w[i] = w[i] + 1;
				if(w[i] == ROADS) {
					w[i] = 0;
					cLaps[i]++;
				}
				CPUKarts[i].setCurrentWayPoint(Vector3(waypoints[w[i]].x + ((rand()%10)-5), waypoints[w[i]].y, waypoints[w[i]].z + ((rand()%10)-5)));
				CPUKarts[i].setRotationY(roads[w[i]].getRotationY());
			}
		}

		if(playerKart.getPosition().z >= waypoints[w[CPU_KARTS]].z -30 && playerKart.getPosition().x >= waypoints[w[CPU_KARTS]].x-30 && playerKart.getPosition().z <= waypoints[w[CPU_KARTS]].z +30 && playerKart.getPosition().x <= waypoints[w[CPU_KARTS]].x+30) {
				w[CPU_KARTS] = w[CPU_KARTS] + 1;
				if(w[CPU_KARTS] == ROADS) {
					laps++;
					w[CPU_KARTS] = 0;
					if(laps == 1) {
						gameOver = true;
						gameStates = endGame;
					}
				}
				playerKart.setCurrentWayPoint(waypoints[w[CPU_KARTS]]);
			}
		if(playerKart.getPosition().z <= waypoints[w[CPU_KARTS]-1].z -30 || playerKart.getPosition().x <= waypoints[w[CPU_KARTS]-1].x-30 && playerKart.getPosition().z >= waypoints[w[CPU_KARTS]-1].z +30 && playerKart.getPosition().x >= waypoints[w[CPU_KARTS]-1].x+30) {
				playerKart.setVelocity(-playerKart.getVelocity()/10);
			}

		for(int i = 0; i < POWER_UPS; i++) {
			if(boosts[i].collided(&playerKart) && !playerKart.getHasBoost()) {
				boosts[i].setInActive();
				playerKart.setHasBoost(true);
			}
		}
		int placementCounter = 0;
		for(int i = 0; i < CPU_KARTS; i++) {
			if(w[CPU_KARTS] > w[i] || (w[CPU_KARTS] <= w[i] && laps > cLaps[i])) {
				placementCounter++;
			}
		}
		playerPosition = CPU_KARTS - placementCounter+1;
		if((GetAsyncKeyState('B') & 0x8000) && playerKart.getHasBoost()){
			audio->playCue(REV);
		}
	}
	else if(gameStates == endGame){
		splash.setPosition(playerKart.getPosition());
		camera.update(dt);
		splash.setRotYAngle(ToRadian(90));
		if(GetAsyncKeyState(VK_RETURN) & 0x8000){
			gameStates = gamePlay;
			//mCarMesh.init(md3dDevice, 1.0f);

			mParallelLight.dir      = D3DXVECTOR3(0.0, -.707f, -.707f);
			mParallelLight.ambient  = D3DXCOLOR(0.4f, 0.4f, 0.5f, 1.0f);
			mParallelLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
				L"../Games_2_GeoRacers/test.png", 0, 0, &mDiffuseMapRV, 0 ));
			HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
				L"../Games_2_GeoRacers/defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));
			HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
				L"../Games_2_GeoRacers/box.png", 0, 0, &boxTexVar, 0 ));
			HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
				L"../Games_2_GeoRacers/boost.png", 0, 0, &boostTexVar, 0 ));
			HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
				L"../Games_2_GeoRacers/stripe.png", 0, 0, &carTexVar, 0 ));

			HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
				L"../Games_2_GeoRacers/WoodCrate01.dds", 0, 0, &splashTex, 0 ));

			spinAmount = 0;

			gameOver = false;

			playerPosition = -1;
			laps = 0;
			boostTimer = 0;

			pKart.init(md3dDevice, 1, CHARCOAL_GREY);
			cKart.init(md3dDevice, 1, RED);
			obstacle.init(md3dDevice, 1, WHITE);

			float angle = 0.0f;

			for(int i = 0;i<ROADS;i++){
				roads[i].init(&road, Vector3(0,0,0), 1);

				Vector3 temp1 = Vector3(0,0,45);

				Matrix m1;

				Identity(&m1);
				RotateY(&m1, ToRadian(angle));

				Vector3 eV;

				Transform(&eV, &temp1,&m1);


				if(i>0)
				{
					Vector3 newPosition = roads[i-1].getPosition() +  eV;
					roads[i].setPosition(newPosition);
				}
				else
				{
					roads[i].setPosition(Vector3(0, -1.2,0));
				}

				roads[i].setRotationY(ToRadian(angle));

				//zPos += roadZLength;
				angle += 1;
				waypoints[i] = roads[i].getPosition();
			}

			D3DXVECTOR3 treeCenters[16];
			for(UINT i = 0; i < 16; ++i)
			{
				float x = RandF(100.0f, 350.0f);
				float z = RandF(100.0f, 350.0f);
				float y = -1.2;

				// Move tree slightly above land height.
				y += 16.0f;

				treeCenters[i] = D3DXVECTOR3(x,y,z);
			}
			mTrees.init(md3dDevice,treeCenters,16);

			playerKart.init(&pKart, 2, Vector3(0,0,0),Vector3(0,0,0),0,1);

			D3DXCOLOR colors [ROADS] = {YELLOW, GREEN, BLUE};


			splash.init(md3dDevice,50.0f,WHITE);

			float roadZLength = 100.0f;

			float zPos = 0;

			float randZPos, randXPos;

			float totalRoadZLength = roadZLength * ROADS;

			float roadXLength = 20;


			float horOffset, fbOffset, powOffset;

			Vector3 tempPosition;

			float maxHorOffset = ROAD_WIDTH/2;
			float minHorOffset = -ROAD_WIDTH/2;

			for(int i = 0; i < OBSTACLES; i++) {
				obstacles[i].init(&obstacle,1.0f,Vector3(0,0,0),Vector3(0,0,0),0,1);

				tempPosition = roads[i].getPosition();

				angle = roads[i].getRotationY();

				horOffset = ((int)maxHorOffset - minHorOffset) * ( (double)rand() / (double)RAND_MAX ) + minHorOffset;

				Vector3 temp1 = Vector3(horOffset,0, 0);

				Matrix m1;

				Identity(&m1);
				RotateY(&m1, ToRadian(angle));

				Vector3 eV;

				Transform(&eV, &temp1,&m1);


				if(i>0)
				{
					Vector3 newPosition1 = roads[i-1].getPosition() +  eV;
					obstacles[i].setPosition(newPosition1);
				}
				else
				{
					obstacles[i].setPosition(roads[i].getPosition() + eV);
				}
				obstacles[i].setRotationY(angle);
			}

			float randVelocity;
			int maxVelocity = PLAYER_MAX_VELOCITY;

			for(int i = 0; i < CPU_KARTS; i++) {
				w[i] = 0;
				randVelocity  = rand() % maxVelocity + 5;
				if(randVelocity > PLAYER_MAX_VELOCITY)
					randVelocity = PLAYER_MAX_VELOCITY +2;
				else if(randVelocity < PLAYER_MAX_VELOCITY*.75)
					randVelocity = PLAYER_MAX_VELOCITY*.80;
				CPUKarts[i].init(&cKart,2,Vector3(0,0,0),Vector3(0,0,randVelocity),0,1);
				CPUKarts[i].setCurrentWayPoint(Vector3(waypoints[w[i]].x + ((rand()%40)-20), waypoints[w[i]].y, waypoints[w[i]].z + ((rand()%40)-20)));
				if (i==0) {
					CPUKarts[i].setPosition(Vector3(playerKart.getPosition().x + 1.5, 0,playerKart.getPosition().z + 2));
				}
				else {
					CPUKarts[i].setPosition(Vector3(CPUKarts[i-1].getPosition().x + 1.5, 0,CPUKarts[i-1].getPosition().z + 2));
				}
			}w[CPU_KARTS] = 0;
			playerKart.setCurrentWayPoint(waypoints[w[CPU_KARTS]]);

			//CAMERA initialization here
			mLight.dir      = D3DXVECTOR3(0.0f, -0.707f, -0.707f);
			mLight.ambient  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
			mLight.diffuse  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
			mLight.specular = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);

			/*mLight2.ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
			mLight2.diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			mLight2.specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			mLight2.att.x    = 0.0f;
			mLight2.att.y    = 0.0f;
			mLight2.att.z    = 1.0f;
			mLight2.spotPow  = 64.0f;
			mLight2.range    = 10000.0f;
			mLight2.pos = D3DXVECTOR3(playerKart.getPosition().x + .25, playerKart.getPosition().y, playerKart.getPosition().z);

			mLight3.ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
			mLight3.diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
			mLight3.specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			mLight3.att.x    = 0.0f;
			mLight3.att.y    = 0.0f;
			mLight3.att.z    = 1.0f;
			mLight3.spotPow  = 64.0f;
			mLight3.range    = 10000.0f;
			mLight3.pos = D3DXVECTOR3(playerKart.getPosition().x + .75, playerKart.getPosition().y, playerKart.getPosition().z);*/

			//camera.init(Vector3(playerKart.getPosition().x + 10,playerKart.getPosition().y + 2,playerKart.getPosition().z), Vector3(0,0,0), Vector3(-1,.2,0));
			camera.init(Vector3(0,2,-10), Vector3(0,0,0), Vector3(0,0,10), &playerKart);
			camera.setPerspective();

			audio = new Audio();
			if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
			{
				if (!audio->initialize()){

				}
			}

			line.init(md3dDevice, 1.0f, WHITE);
			xLine.init(&line, Vector3(0,0,0), 5);
			xLine.setPosition(Vector3(0,0,0));
			yLine.init(&line, Vector3(0,0,0), 5);
			yLine.setPosition(Vector3(0,0,0));
			yLine.setRotationZ(ToRadian(90));
			zLine.init(&line, Vector3(0,0,0), 5);
			zLine.setPosition(Vector3(0,0,0));
			zLine.setRotationY(ToRadian(270));

			//gameStates = gameMenu
		}
	}
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();
	//Camera view and projection matrices
	mView = camera.getViewMatrix();
	mProj = camera.getProjectionMatrix();
	if(gameStates == gamePlay)
	{

		md3dDevice->OMSetDepthStencilState(0, 0);
		float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
		md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
		md3dDevice->IASetInputLayout(mVertexLayout);
		mfxEyePosVar->SetRawValue(&camera.getPosition(), 0, sizeof(D3DXVECTOR3));
		mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));

		mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
		mfxSpecMapVar->SetResource(mSpecMapRV);


		mWVP = xLine.getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)xLine.getWorldMatrix());
		xLine.setMTech(mTech);
		xLine.draw();

		mWVP = yLine.getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)yLine.getWorldMatrix());
		yLine.setMTech(mTech);
		yLine.draw();

		mWVP = zLine.getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)zLine.getWorldMatrix());
		zLine.setMTech(mTech);
		zLine.draw();
		mfxEyePosVar->SetRawValue(&camera.getPosition(), 0, sizeof(D3DXVECTOR3));
		mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
		mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
		mfxSpecMapVar->SetResource(mSpecMapRV);
		D3DXMATRIX texMtx;
		D3DXMatrixIdentity(&texMtx);
		mfxTexMtxVar->SetMatrix((float*)&texMtx);



		/*mfxLightVar->SetRawValue(&mLight, 0, sizeof(Light));*/
		//mfxLightType->SetInt(0);


		D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc(&techDesc);
		

		for(int i = 0;i<ROADS;i++)
		{
			mWVP = roads[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxWorldVar->SetMatrix((float*)roads[i].getWorldMatrix());
			roads[i].setMTech(mTech);
			roads[i].draw();
		}

		mfxEyePosVar->SetRawValue(&camera.getPosition(), 0, sizeof(D3DXVECTOR3));
		mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
		mfxDiffuseMapVar->SetResource(carTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);
		D3DXMatrixIdentity(&texMtx);
		mfxTexMtxVar->SetMatrix((float*)&texMtx);



		/*mfxLightVar->SetRawValue(&mLight, 0, sizeof(Light));*/
		//mfxLightType->SetInt(0);


		mTech->GetDesc(&techDesc);

		for(int i = 0; i < CPU_KARTS; i++)
		{
			mWVP = CPUKarts[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxWorldVar->SetMatrix((float*)CPUKarts[i].getWorldMatrix());
			CPUKarts[i].setMTech(mTech);
			CPUKarts[i].draw();
		}
		mfxDiffuseMapVar->SetResource(boxTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);

		for(int i = 0; i < OBSTACLES; i++)
		{
			mWVP = obstacles[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxWorldVar->SetMatrix((float*)obstacles[i].getWorldMatrix());
			obstacles[i].setMTech(mTech);
			obstacles[i].draw();
		}
		mfxDiffuseMapVar->SetResource(boostTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);
		for(int i = 0; i < POWER_UPS; i++)
		{
			mWVP = boosts[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxWorldVar->SetMatrix((float*)boosts[i].getWorldMatrix());
			boosts[i].setMTech(mTech);
			if(boosts[i].getActiveState())
				boosts[i].draw();
		}
		mfxEyePosVar->SetRawValue(&camera.getPosition(), 0, sizeof(D3DXVECTOR3));
		mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
		mfxDiffuseMapVar->SetResource(carTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);
		D3DXMatrixIdentity(&texMtx);
		mfxTexMtxVar->SetMatrix((float*)&texMtx);



		/*mfxLightVar->SetRawValue(&mLight, 0, sizeof(Light));*/
		//mfxLightType->SetInt(0);


		mTech->GetDesc(&techDesc);
		mWVP = playerKart.getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)playerKart.getWorldMatrix());
		playerKart.setMTech(mTech);
		playerKart.draw();

		std::wstring playerPositionText, gameOverText;

		std::wostringstream outs;   
		outs.precision(6);
		switch (playerPosition)
		{
		case 1:
			outs << playerPosition << "st";
			break;
		case 2: 
			outs << playerPosition << "nd";
			break;
		case 3: 
			outs << playerPosition << "rd";
			break;
		default:
			outs << playerPosition << "th";
			break;
		}
		playerPositionText = outs.str();

		//bills
		// Draw tree sprites, which use their own fx.

		mTrees.draw(mParallelLight, camera.getPosition(), mView*mProj);
		md3dDevice->RSSetState(0); // restore default

		// We specify DT_NOCLIP, so we do not care about width/height of the rect.
		RECT playerPos = {mClientWidth-85, 10, mClientWidth, mClientHeight};
		mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);

	}
	else if (gameStates==gameMenu) {
		std::wstring playerPositionText, gameOverText;

		std::wostringstream outs;   
		outs.precision(6);
		outs << "Geo Racers\n\n\n\n\nPress <enter> to begin!";
		playerPositionText = outs.str();
		RECT playerPos = {100, 100, mClientWidth, mClientHeight};
		mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);

		std::wostringstream outs2;   
		outs.precision(6);
		outs2 << "\n\nControls:\n-W/S: Accelerate/Decelerate\n-A/D: Left/Right\n-Space: Jump\n-Collect a power up and press B to boost";
		playerPositionText = outs2.str();
		smallFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);


	}
	else if (gameStates==endGame) {

		std::wstring playerPositionText, gameOverText;

		std::wostringstream outs;   
		outs.precision(6);
		RECT playerPos = {100, 100, mClientWidth, mClientHeight};
		switch(playerPosition) {
		case 1:
			outs << "FINISH!\n" << playerPosition << "st Place!\n\nPress <enter> to continue!";
			playerPositionText = outs.str();
			mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);
			break;
		case 2:
			outs << "FINISH!\n" << playerPosition << "nd Place!\n\nPress <enter> to continue!";
			playerPositionText = outs.str();
			mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);
			break;
		case 3:
			outs << "FINISH!\n" << playerPosition << "rd Place.\n\nPress <enter> to continue!";
			playerPositionText = outs.str();
			mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);
			break;
		case 6:
			outs << "FINISH!\nLast Place...\n\nPress <enter> to continue!";
			playerPositionText = outs.str();
			mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);
			break;
		default:
			outs << "FINISH!\n" << playerPosition << "th Place.\n\nPress <enter> to continue!";
			playerPositionText = outs.str();
			mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);
			break;
		}

		//	D3DXMATRIX texMtx;
		//	D3DXMatrixIdentity(&texMtx);
		//	mfxTexMtxVar->SetMatrix((float*)&texMtx);


		//	mfxEyePosVar->SetRawValue(cameraPos, 0, sizeof(D3DXVECTOR3));
		//	mfxLightVar->SetRawValue(&mLight, 0, sizeof(Light));
		////	mfxLightType->SetInt(0);


		//	D3D10_TECHNIQUE_DESC techDesc;
		//	mTech->GetDesc(&techDesc);
		//	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
		//	mWVP = playerKart.getWorldMatrix()*mView*mProj;
		//	mfxWVPVar->SetMatrix((float*)&mWVP);
		//	playerKart.setMTech(mTech);
		//	mfxDiffuseMapVar->SetResource(splashTex);
		//	mfxSpecMapVar->SetResource(mSpecMapRV);
		//	playerKart.draw();
		//	splash.draw();

	}

	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"../Games_2_GeoRacers/tex.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("TexTech");

	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxLightVar      = mFX->GetVariableByName("gLight");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	/*hr = D3DX10CreateEffectFromFile(L"../Games_2_GeoRacers/lighting.fx", 0, 0, 
	"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
	if( compilationErrors )
	{
	MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
	ReleaseCOM(compilationErrors);
	}
	DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("LightTech");

	mfxWVPVar    = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar  = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
	mfxLightVar  = mFX->GetVariableByName("gLight");
	mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();*/

}


void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	//	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	//	{
	//		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
	//		//{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
	//		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
	//		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	//		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}
	//	};
	//
	//	// Create the input layout
	//	D3D10_PASS_DESC PassDesc;
	//	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	//	HR(md3dDevice->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,
	//		PassDesc.IAInputSignatureSize, &mVertexLayout));
	//}
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}


