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
#include "Line.h"
#include <d3dx9math.h>
#include "Camera.h"
#include "LineObject.h"
#include "Quad.h"
#include "Road.h"


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
 
private:
// Camera stuff
	Vector3 cameraPos;
	Vector3 lookAt;
	Camera camera;

	Line line;
	//Add line, box, and gameobject definitions here
	Box pKart, cKart;
	GameObject playerKart;
	GameObject CPUKarts[7];

	Road road[ROADS];
	LineObject xLine, yLine, zLine;

	float spinAmount;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;

	ID3D10EffectVariable* mfxFLIPVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;


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

	//Add box, line, and gameobject initialization here
	
	buildFX();
	buildVertexLayouts();

	spinAmount = 0;

	pKart.init(md3dDevice, 1, BLUE);
	cKart.init(md3dDevice, 1, RED);

	playerKart.init(&pKart, 2, Vector3(0,0,0),Vector3(0,0,-3),0,1);

	for(int i = 0; i < 7; i++) {
		CPUKarts[i].init(&cKart,2,Vector3(0,0,0),Vector3(0,0,-3),0,1);
		if (i==0) {
			CPUKarts[i].setPosition(Vector3(playerKart.getPosition().x + 1.5, 0,playerKart.getPosition().z + 2));
		}
		else {
			CPUKarts[i].setPosition(Vector3(CPUKarts[i-1].getPosition().x + 1.5, 0,CPUKarts[i-1].getPosition().z + 2));
		}
	}

	//CAMERA initialization here
	camera.init(Vector3(0,0,0), Vector3(0,0,0), Vector3(0,0,10));
	camera.setPerspective();

	line.init(md3dDevice, 1.0f, WHITE);
	xLine.init(&line, Vector3(0,0,0), 5);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&line, Vector3(0,0,0), 5);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&line, Vector3(0,0,0), 5);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(270));

	D3DXCOLOR colors [ROADS] = {YELLOW, GREEN, BLUE};

	float roadZLength = 100.0f;

	float zPos = 0;

	for(int i = 0;i<ROADS;i++)
	{
		road[i].init(md3dDevice,1, colors[i]);
		road[i].setPosition(Vector3(0,-1.2,zPos));
		zPos += roadZLength;
	}

	

}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();
	camera.setPerspective();
	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);

	for(int i = 0;i<ROADS;i++)
	{
		road[i].update(dt);
	}

	camera.update(dt);
	//ADD UPDATES HERE
	playerKart.update(dt);
	for (int i = 0; i < 7; i++) {
		CPUKarts[i].update(dt);
	}
	
	spinAmount += 1*dt;
	if (spinAmount > 360)
		spinAmount = 0;

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

	 for(int i = 0; i < 7; i++)
	{
		mWVP = CPUKarts[i].getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		CPUKarts[i].setMTech(mTech);
		CPUKarts[i].draw();
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
	mfxFLIPVar = mFX->GetVariableByName("flip");

}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 