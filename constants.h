#ifndef Constants_H
#define Constants_H

//Defines to make life easier

#define Vector3 D3DXVECTOR3
#define Matrix D3DXMATRIX
#define Identity D3DXMatrixIdentity
#define Translate D3DXMatrixTranslation
#define RotateX D3DXMatrixRotationX
#define RotateY D3DXMatrixRotationY
#define RotateZ D3DXMatrixRotationZ
#define ToRadian D3DXToRadian
#define ToDegree D3DXToDegree
#define Transform D3DXVec3TransformCoord

const int ROADS = 6;
const int OBSTACLES = 20;
const int CPU_KARTS = 3;

const char WAVE_BANK[]  = "..\\Games_2_GeoRacers\\audio\\Win\\Wave Bank.xwb";
// SOUND_BANK must be location of .xsb file.
const char SOUND_BANK[] = "..\\Games_2_GeoRacers\\audio\\Win\\Sound Bank.xsb";

const char REV[] = "Rev";
const char SQUEAL[] = "Squeal";
const char CIRCUIT[] = "Circuit";
const float PLAYER_ACCELERATION = 0.3f;
const float PLAYER_MAX_VELOCITY = 20.0f;
const float ROAD_WIDTH = 20.0f;
const float ROAD_LENGTH = 100.0f;

enum GameStates {gameMenu, gamePlay, endGame};





#endif