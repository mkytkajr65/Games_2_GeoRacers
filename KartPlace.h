
#ifndef KartPlace_H
#define KartPlace_H

#include "GameObject.h"
#include "d3dApp.h"

class KartPlace
{
public:
	static GameObject* getKartsPlaces(GameObject *Karts, int size);
	
	static int getPlayerPosition(GameObject *Karts, int size);
};


#endif