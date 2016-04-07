//=======================================================================================
// Box.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Box.h"
#include "Vertex.h"

Box::Box()
: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
}
 
Box::~Box()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}
void Box::init(ID3D10Device* device, float scale, D3DXCOLOR c)
{
	md3dDevice = device;
 
	mNumVertices = 16;
	mNumFaces    = 24; // 2 per quad

	// Create vertex buffer
    Vertex vertices[] =
    {
		{D3DXVECTOR3(-0.5f, -.5f, -2.0f), c},
		{D3DXVECTOR3(-0.5f, +.5f, -2.0f), c},
		{D3DXVECTOR3(+0.5f, +.5f, -2.0f), c},
		{D3DXVECTOR3(+0.5f, -.5f, -2.0f), c},
		{D3DXVECTOR3(-0.5f, -.5f, +2.0f), c},
		{D3DXVECTOR3(-0.5f, +.5f, +2.0f), c},
		{D3DXVECTOR3(+0.5f, +.5f, +2.0f), c},
		{D3DXVECTOR3(+0.5f, -.5f, +2.0f), c},
		
		{D3DXVECTOR3(-0.5f, -.5f, -0.0f), c},
		{D3DXVECTOR3(-0.5f, +.0f, -0.0f), c},
		{D3DXVECTOR3(+0.5f, +.0f, -0.0f), c},
		{D3DXVECTOR3(+0.5f, -.5f, -0.0f), c},
		{D3DXVECTOR3(-0.5f, -.5f, +3.0f), c},
		{D3DXVECTOR3(-0.5f, +.0f, +3.0f), c},
		{D3DXVECTOR3(+0.5f, +.0f, +3.0f), c},
		{D3DXVECTOR3(+0.5f, -.5f, +3.0f), c},
    };


    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer

	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7,

		//second
		8, 9, 10,
		8, 10, 11,

		// back face
		12, 14, 13,
		12, 15, 14,

		// left face
		12, 13, 9,
		12, 9, 8,

		// right face
		11, 10, 14,
		11, 14, 15,

		// top face
		9, 13, 14,
		9, 14, 10,

		// bottom face
		12, 8, 11, 
		12, 11, 15
	};

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}
void Box::init(ID3D10Device* device, float scale)
{
	md3dDevice = device;
 
	mNumVertices = 8;
	mNumFaces    = 12; // 2 per quad

	// Create vertex buffer
    Vertex vertices[] =
    {
		{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), WHITE},
		{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), BLACK},
		{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), RED},
		{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), GREEN},
		{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), BLUE},
		{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), YELLOW},
		{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), CYAN},
		{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), MAGENTA},
    };



    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer

	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Box::draw()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
}