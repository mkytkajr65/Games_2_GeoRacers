

#ifndef VERTEX_H
#define VERTEX_H

//#include "d3dUtil.h"
#include "d3dUtil.h"

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 norm;
	D3DXCOLOR diff;
	D3DXCOLOR spec;
	D3DXVECTOR2 tex;
};
 
#endif // VERTEX_H