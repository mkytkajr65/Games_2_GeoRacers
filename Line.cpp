
#include "Line.h"
#include "Vertex.h"
#include "constants.h"

Line::Line()
: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	Identity(&world);
}
 
Line::~Line()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}
void Line::init(ID3D10Device* device, float scale, D3DXCOLOR c)
{
	md3dDevice = device;
 
	mNumVertices = 2;
	mNumFaces    = 1; 

	// Create vertex buffer
    Vertex vertices[] =
    {
		{D3DXVECTOR3(0.0f, 0.0f, 0.0f),D3DXVECTOR3(0, 0, 0),D3DXVECTOR2(0,0)},
		{D3DXVECTOR3(2.0f, 0.0f, 0.0f),D3DXVECTOR3(0, 0, 0),D3DXVECTOR2(0,0)}

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
}

void Line::update(float dt)
{
	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;
}

void Line::draw()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	md3dDevice->Draw(2,0);
}