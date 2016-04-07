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
#include "KartPlace.h"
#include "Light.h"
#include "audio.h"


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

	Line line;
	//Add line, box, and gameobject definitions here
	Box pKart, cKart;
	Obstacle obstacle;
	GameObject playerKart;
	CPUKartObject CPUKarts[CPU_KARTS];

	ObstacleObject obstacles[OBSTACLES];

	Road road[ROADS];
	LineObject xLine, yLine, zLine;
	Light mLight;
	Audio *audio;
	/*Light mLight2;
	Light mLight3;*/

	float spinAmount;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;


	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
	float soundTimer;
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
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f)
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

	spinAmount = 0;
	soundTimer = 0;
	pKart.init(md3dDevice, 1, CHARCOAL_GREY);
	cKart.init(md3dDevice, 1, RED);
	obstacle.init(md3dDevice, 1, WHITE);

	playerKart.init(&pKart, 2, Vector3(0,0,0),Vector3(0,0,0),0,1);

	D3DXCOLOR colors [ROADS] = {YELLOW, GREEN, BLUE};

	float roadZLength = 100.0f;

	float zPos = 0;

	for(int i = 0;i<ROADS;i++)
	{
		road[i].init(md3dDevice,1, colors[i]);
		road[i].setPosition(Vector3(0,-1.2,zPos));
		zPos += roadZLength;
	}

	float randZPos, randXPos;

	float totalRoadZLength = roadZLength * ROADS;

	float roadXLength = 20;

	for(int i = 0; i < OBSTACLES; i++) {
		obstacles[i].init(&obstacle,0,Vector3(0,0,0),Vector3(0,0,0),0,1);

		randZPos = ((int)totalRoadZLength - 25) * ( (double)rand() / (double)RAND_MAX ) + 25;
		randXPos = (rand() % (int)roadXLength)-10;

		obstacles[i].setPosition(Vector3(randXPos, -1.2, randZPos));
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
	D3DApp::updateScene(dt);
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);

	float velX, velY, velZ;

	velX = playerKart.getVelocity().x;
	for(int i = 0; i < ROADS; i++) {
		if(playerKart.getPosition().x <= road[i].getPosition().x -10 && velX < 0)
			velX = -velX/2;
		if(playerKart.getPosition().x >= road[i].getPosition().x + 10 && velX > 0)
			velX = -velX/2;
	}
	velY = 0.0;
	velZ = playerKart.getVelocity().z;

	//ADD UPDATES HERE
	Vector3 direction = Vector3(0,0,0);
	if(GetAsyncKeyState('A') & 0x8000){
			direction.x = -1;
			velX = velX - PLAYER_ACCELERATION;
	}
	if(GetAsyncKeyState('D') & 0x8000){
			direction.x = 1;
			velX = velX + PLAYER_ACCELERATION;
	}
	if(GetAsyncKeyState('S') & 0x8000){
			direction.z = -1;
	if(soundTimer >= 2) {
		soundTimer = 0;
		audio->stopCue(REV);
	}
			velZ = velZ - PLAYER_ACCELERATION;
	}
	if(GetAsyncKeyState('W') & 0x8000){
			direction.z = 1;
			velZ = velZ + PLAYER_ACCELERATION;
			soundTimer += dt;
			if(soundTimer >= 1)
				audio->playCue(REV);
	}
	

	

	if(velX > PLAYER_MAX_VELOCITY){
		velX = PLAYER_MAX_VELOCITY ;
	}else if(velX < -PLAYER_MAX_VELOCITY){
		velX = -PLAYER_MAX_VELOCITY ;
	}

	if(velZ > PLAYER_MAX_VELOCITY){
		velZ = PLAYER_MAX_VELOCITY ;
	}else if(velZ < -PLAYER_MAX_VELOCITY){
		velZ = -PLAYER_MAX_VELOCITY ;
	}



	_RPT1(0,"Velocity X %f ", velX);
	_RPT1(0,"Velocity Z %f ", velZ);

	D3DXVec3Normalize(&direction, &direction);

	Vector3 playerVelocity = Vector3(velX, velY, velZ);

	playerKart.setVelocity(playerVelocity);

	camera.update(dt, playerVelocity);

	playerKart.update(dt);
	for (int i = 0; i < CPU_KARTS; i++) {
		CPUKarts[i].update(dt);
	}

	for (int i = 0; i < OBSTACLES; i++) {
		obstacles[i].update(dt);
	}
	for(int i = 0;i<ROADS;i++)
	{
		road[i].update(dt);
	}

	for(int i = 0;i<OBSTACLES;i++)
	{
		if(playerKart.collided(&obstacles[i]) && !playerKart.getAlreadyCollided())
		{
			playerKart.setAlreadyCollided(true);
			playerKart.setVelocity(Vector3(0,0,0));
			audio->playCue(SQUEAL);
		}
		else if(!playerKart.collided(&obstacles[i]) && !playerKart.getAlreadyCollided()){
			playerKart.setAlreadyCollided(false);
		}
	}

	for(int j = 0;j<CPU_KARTS;j++)
	{
		for(int i = 0;i<OBSTACLES;i++)
		{
			if(CPUKarts[j].collided(&obstacles[i]) && !CPUKarts[j].getAlreadyCollided())
			{
				CPUKarts[j].setAlreadyCollided(true);
				CPUKarts[j].setVelocity(Vector3(0,0,0));
			}else if(!CPUKarts[j].collided(&obstacles[i]) && !CPUKarts[j].getAlreadyCollided()){
				CPUKarts[j].setAlreadyCollided(false);
			}
		}
	}
	KartPlace place;
	GameObject allKarts[CPU_KARTS+1];

	for(int i = 0;i<CPU_KARTS;i++)
	{
		allKarts[i] = CPUKarts[i];
	}

	allKarts[CPU_KARTS] = playerKart;
	/*mLight2.pos = D3DXVECTOR3(playerKart.getPosition().x + .25, playerKart.getPosition().y, playerKart.getPosition().z);
	mLight3.pos = D3DXVECTOR3(playerKart.getPosition().x + .75, playerKart.getPosition().y, playerKart.getPosition().z);*/

	GameObject* places = place.getKartsPlaces(allKarts, CPU_KARTS+1);

	//place.printTopThree(places, CPU_KARTS+1);
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();

	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

	//Camera view and projection matrices
	 mView = camera.getViewMatrix();
	 mProj = camera.getProjectionMatrix();

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

	mfxEyePosVar->SetRawValue(cameraPos, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mLight, 0, sizeof(Light));
	mfxLightType->SetInt(0);

	/*mfxEyePosVar->SetRawValue(cameraPos, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mLight2, 0, sizeof(Light));
	mfxLightType->SetInt(2);

	mfxEyePosVar->SetRawValue(cameraPos, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mLight3, 0, sizeof(Light));
	mfxLightType->SetInt(2);*/

	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	for(int i = 0;i<ROADS;i++)
	{
		mWVP = road[i].getWorld()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mTech->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			mTech->GetPassByIndex( p )->Apply(0);
			road[i].draw();
		}
	}

	for(int i = 0; i < CPU_KARTS; i++)
	{
		mWVP = CPUKarts[i].getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		CPUKarts[i].setMTech(mTech);
		CPUKarts[i].draw();
	}

	for(int i = 0; i < OBSTACLES; i++)
	{
		mWVP = obstacles[i].getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		obstacles[i].setMTech(mTech);
		obstacles[i].draw();
	}
	
	 mWVP = playerKart.getWorldMatrix()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	playerKart.setMTech(mTech);
	playerKart.draw();

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

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
	hr = D3DX10CreateEffectFromFile(L"../Games_2_GeoRacers/color.fx", 0, 0, 
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

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	hr = D3DX10CreateEffectFromFile(L"../Games_2_GeoRacers/lighting.fx", 0, 0, 
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
	mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		//{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 4, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 