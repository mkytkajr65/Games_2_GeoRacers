#include "KartPlace.h"

GameObject* KartPlace::getKartsPlaces(GameObject *karts, int size)
{
	GameObject* kartsByPosition = new GameObject[size];
	for(int i = 0;i<size;i++)
	{
		kartsByPosition[i] = karts[i];
	}
	for(int i = 0;i<4;i++){
		_RPT1(0,"*type* %c ", kartsByPosition[i].type());
	}
	GameObject temp; //for swapping
    for (int i = 0 ; i < size ; i++)
    {
        for (int j = 0 ; j < size - 1 ; j++)
        {
			if ( kartsByPosition[j].getPosition().z < kartsByPosition[j+1].getPosition().z )
            {
                temp = kartsByPosition[j];
                kartsByPosition[j] = kartsByPosition[j+1];
                kartsByPosition[j+1] = temp;
            }
        }
    }
	for(int i = 0;i<4;i++){
		_RPT1(0,"*type* %c ", kartsByPosition[i].type());
	}
	return kartsByPosition;
}

int KartPlace::getPlayerPosition(GameObject *Karts, int size){
	for(int i = 0;i< size;i++)
	{
		if(Karts[i].type()=='p'){
			return i + 1;
		}
	}
	return -1;
}