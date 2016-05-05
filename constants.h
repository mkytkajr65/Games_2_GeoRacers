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

const int ROADS = 360;
const int OBSTACLES = ROADS;
const int CPU_KARTS = 3;
const int POWER_UPS = ROADS;

const char WAVE_BANK[]  = "..\\Games_2_GeoRacers\\audio\\Win\\Wave Bank.xwb";
// SOUND_BANK must be location of .xsb file.
const char SOUND_BANK[] = "..\\Games_2_GeoRacers\\audio\\Win\\Sound Bank.xsb";

const char REV[] = "Rev";
const char SQUEAL[] = "Squeal";
const char CIRCUIT[] = "Circuit";
const float PLAYER_MAX_ACCELERATION = 0.1f;
const float PLAYER_MAX_VELOCITY = 200.0f;
const float INITIAL_PLAYER_JUMP_VELOCITY = 30.0f;
const float GRAVITY = 0.1f;
const float ROAD_WIDTH = 50.0f;
const float ROAD_LENGTH = 50.0f;
const float BOOST_VELOCITY = 400.0f;
const float MAX_BOOST_TIME = 3.0f;

enum GameStates {gameMenu, gamePlay, endGame};





#endif