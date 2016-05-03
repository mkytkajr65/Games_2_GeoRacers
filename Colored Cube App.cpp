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


	TreeSprites mTrees;
	Road road;
	RoadObject roads[ROADS];
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

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
	float boostTimer;
	float mTheta;
	float mPhi;
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
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	buildFX();
	buildVertexLayouts();

	revPlaying = false;

	gameStates = gameMenu;
	//mCarMesh.init(md3dDevice, 1.0f);



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
	mParallelLight.dir      = D3DXVECTOR3(0.57735f, -0.57735f, -0.57735f);
	mParallelLight.ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mParallelLight.diffuse  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
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

	float randTranslate;

	for(int i = 0; i < OBSTACLES; i++) {
		obstacles[i].init(&obstacle,1.0f,Vector3(0,0,0),Vector3(0,0,0),0,1);

			/*randZPos = ((int)totalRoadZLength - 25) * ( (double)rand() / (double)RAND_MAX ) + 25;
			randXPos = (rand() % (int)roadXLength)-10;*/
			randTranslate = ((int)45 - 20) * ( (double)rand() / (double)RAND_MAX ) + 20;

			Vector3 temp1 = Vector3(0,0,randTranslate);

			Matrix m1;

			Identity(&m1);
			RotateY(&m1, ToRadian(angle));

			Vector3 eV;

			Transform(&eV, &temp1,&m1);


			if(i>0){
			Vector3 newPosition = obstacles[i-1].getPosition() +  eV;
			obstacles[i].setPosition(newPosition);
			}else{
			obstacles[i].setPosition(Vector3(0, -1.2,0));
			}

			obstacles[i].setRotationY(ToRadian(angle));

			//zPos += roadZLength;
			angle += 1;

		//obstacles[i].setPosition(Vector3(randXPos, -1.2, randZPos));
	}


	for(int i = 0; i < POWER_UPS; i++) {
		boosts[i].init(&boostBox,1.0f,Vector3(0,0,0),Vector3(0,0,0),0,1);

		randZPos = ((int)totalRoadZLength - 25) * ( (double)rand() / (double)RAND_MAX ) + 25;
		randXPos = (rand() % (int)roadXLength)-10;

		boosts[i].setPosition(Vector3(randXPos, -1.2, randZPos));
	}
	float randVelocity;
	int maxVelocity = PLAYER_MAX_VELOCITY;

	for(int i = 0; i < CPU_KARTS; i++) {
		randVelocity  = rand() % maxVelocity + 5;
		if(randVelocity > PLAYER_MAX_VELOCITY)
			randVelocity = PLAYER_MAX_VELOCITY - 1;
		else if(randVelocity < PLAYER_MAX_VELOCITY*.75)
			randVelocity = PLAYER_MAX_VELOCITY*.75;
		CPUKarts[i].init(&cKart,2,Vector3(0,0,0),Vector3(0,0,randVelocity),0,1);
		if (i==0) {
			CPUKarts[i].setPosition(Vector3(playerKart.getPosition().x + 1.5, 0,playerKart.getPosition().z + 2));
		}
		else {
			CPUKarts[i].setPosition(Vector3(CPUKarts[i-1].getPosition().x + 1.5, 0,CPUKarts[i-1].getPosition().z + 2));
		}
	}

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


		for(int i = 0;i<CPU_KARTS;i++)
		{
			CPUKarts[i].update(dt);
		}

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
		/*for (int j = 0; j < CPU_KARTS; j++) {
		for(int i = 0;i<OBSTACLES;i++)
		{
		if(CPUKarts[j].collided(&obstacles[i]) && !CPUKarts[j].getAlreadyCollided())
		{
		CPUKarts[j].setAlreadyCollided(true);
		CPUKarts[j].setVelocity(CPUKarts[j].getVelocity()/2);
		audio->playCue(SQUEAL);
		}else if(!CPUKarts[j].collided(&obstacles[i]) && CPUKarts[j].getAlreadyCollided()){
		CPUKarts[j].setAlreadyCollided(false);
		CPUKarts[j].setVelocity(CPUKarts[j].getVelocity()*2);  
		}
		}
		}*/

		for(int i = 0; i < POWER_UPS; i++) {
			if(boosts[i].collided(&playerKart) && !playerKart.getHasBoost()) {
				boosts[i].setInActive();
				playerKart.setHasBoost(true);
				audio->playCue(REV);
			}
		}

		/*KartPlace place;
		GameObject* allKarts = new GameObject[CPU_KARTS+1];


		//KartPlace place;
		//GameObject allKarts[CPU_KARTS+1];

		for(int i = 0;i<CPU_KARTS;i++)
		{
		allKarts[i] = CPUKarts[i];
		}

		allKarts[CPU_KARTS] = playerKart;*/
		/*mLight2.pos = D3DXVECTOR3(playerKart.getPosition().x + .25, playerKart.getPosition().y, playerKart.getPosition().z);
		mLight3.pos = D3DXVECTOR3(playerKart.getPosition().x + .75, playerKart.getPosition().y, playerKart.getPosition().z);*/

		/*GameObject* places = place.getKartsPlaces(allKarts, CPU_KARTS+1);

		playerPosition = place.getPlayerPosition(places, CPU_KARTS+1);

		for(int i = 0;i<CPU_KARTS+1;i++){
		if(allKarts[i].getPosition().z + 4.0f >= (ROADS * ROAD_LENGTH)){
		gameOver = true;
		gameStates = endGame;
		}
		//place.printTopThree(places, CPU_KARTS+1);
		}*/


	}
	else if(gameStates == endGame){
		splash.setPosition(playerKart.getPosition());
		camera.update(dt);
		splash.setRotYAngle(ToRadian(90));
		if(GetAsyncKeyState(VK_RETURN) & 0x8000){
			gameStates = gamePlay;
			//mCarMesh.init(md3dDevice, 1.0f);

			mParallelLight.dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
			mParallelLight.ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
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

			boostTimer = 0;

			pKart.init(md3dDevice, 1, CHARCOAL_GREY);
			cKart.init(md3dDevice, 1, RED);
			obstacle.init(md3dDevice, 1, WHITE);



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

			for(int i = 0; i < OBSTACLES; i++) {
				obstacles[i].init(&obstacle,0,Vector3(0,0,0),Vector3(0,0,0),0,1);

				randZPos = ((int)totalRoadZLength - 25) * ( (double)rand() / (double)RAND_MAX ) + 25;
				randXPos = (rand() % (int)roadXLength)-10;

				obstacles[i].setPosition(Vector3(randXPos, -1.2, randZPos));
			}

			for(int i = 0; i < POWER_UPS; i++) {
				boosts[i].init(&boostBox,1.0f,Vector3(0,0,0),Vector3(0,0,0),0,1);

				randZPos = ((int)totalRoadZLength - 25) * ( (double)rand() / (double)RAND_MAX ) + 25;
				randXPos = (rand() % (int)roadXLength)-10;

				boosts[i].setPosition(Vector3(randXPos, -1.2, randZPos));
			}

			float randVelocity;
			int maxVelocity = 10.0;

			for(int i = 0; i < CPU_KARTS; i++) {
				randVelocity  = rand() % maxVelocity + 1;
				CPUKarts[i].init(&cKart,2,Vector3(0,0,0),Vector3(0,0,randVelocity),0,1);
				if (i==0) {
					CPUKarts[i].setPosition(Vector3(playerKart.getPosition().x + 1.5, 0,playerKart.getPosition().z + 2));
				}
				else {
					CPUKarts[i].setPosition(Vector3(CPUKarts[i-1].getPosition().x + 1.5, 0,CPUKarts[i-1].getPosition().z + 2));
				}
			}

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
		mfxEyePosVar->SetRawValue(cameraPos, 0, sizeof(D3DXVECTOR3));
		mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));

		mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
		mfxSpecMapVar->SetResource(mSpecMapRV);

		md3dDevice->OMSetDepthStencilState(0, 0);
		float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
		md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
		md3dDevice->IASetInputLayout(mVertexLayout);


		mWVP = xLine.getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		xLine.setMTech(mTech);
		xLine.draw();

		mWVP = yLine.getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		yLine.setMTech(mTech);
		yLine.draw();

		mWVP = zLine.getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		zLine.setMTech(mTech);
		zLine.draw();

		D3DXMATRIX texMtx;
		D3DXMatrixIdentity(&texMtx);
		mfxTexMtxVar->SetMatrix((float*)&texMtx);



		/*mfxLightVar->SetRawValue(&mLight, 0, sizeof(Light));*/
		//mfxLightType->SetInt(0);


		D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc(&techDesc);

		mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
		mfxSpecMapVar->SetResource(mSpecMapRV);

		for(int i = 0;i<ROADS;i++)
		{
			mWVP = roads[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			roads[i].setMTech(mTech);
			roads[i].draw();
		}


		mfxDiffuseMapVar->SetResource(carTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);

		for(int i = 0; i < CPU_KARTS; i++)
		{
			mWVP = CPUKarts[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			CPUKarts[i].setMTech(mTech);
			CPUKarts[i].draw();
		}
		mfxDiffuseMapVar->SetResource(boxTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);

		for(int i = 0; i < OBSTACLES; i++)
		{
			mWVP = obstacles[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			obstacles[i].setMTech(mTech);
			obstacles[i].draw();
		}
		mfxDiffuseMapVar->SetResource(boostTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);
		for(int i = 0; i < POWER_UPS; i++)
		{
			mWVP = boosts[i].getWorldMatrix()*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			boosts[i].setMTech(mTech);
			if(boosts[i].getActiveState())
				boosts[i].draw();
		}
		mfxDiffuseMapVar->SetResource(carTexVar);
		mfxSpecMapVar->SetResource(mSpecMapRV);
		mWVP = playerKart.getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
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
		outs << "Geo Racers\n\n\nPress <enter> to begin!";
		playerPositionText = outs.str();
		RECT playerPos = {100, 100, mClientWidth, mClientHeight};
		mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);


	}
	else if (gameStates==endGame) {

		std::wstring playerPositionText, gameOverText;

		std::wostringstream outs;   
		outs.precision(6);
		outs << "FINISH!\n\n\nPress <enter> to continue!";
		playerPositionText = outs.str();
		RECT playerPos = {100, 100, mClientWidth, mClientHeight};
		mFont->DrawText(0, playerPositionText.c_str(), -1, &playerPos, DT_NOCLIP, WHITE);

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

	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
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
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 56, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 5, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}


