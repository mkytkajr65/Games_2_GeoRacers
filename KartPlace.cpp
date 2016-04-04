#include "KartPlace.h"

GameObject* KartPlace::getKartsPlaces(GameObject *karts, int size)
{
	GameObject *kartsByPosition = new GameObject[size];
	for(int i = 0;i<size;i++)
	{
		kartsByPosition[i] = karts[i];
	}
	bool swapOccurred = true;
	for(int i = 0;i<=size && swapOccurred;i++)
	{
		swapOccurred = false;
		for(int j = 0;j<size-1;j++)
		{
			//_RPT1(0,"z Value %f ", kartsByPosition[j+1].getPosition().z);
			int second = kartsByPosition[j+1].getPosition().z;
			int first = kartsByPosition[j].getPosition().z;
			if(first < second)
			{
				GameObject temp = kartsByPosition[j];
				kartsByPosition[j] = kartsByPosition[j+1];
				kartsByPosition[j+1] = temp;
				swapOccurred = true;
			}
		}
	}
	return kartsByPosition;
}

void KartPlace::printTopThree(GameObject *karts, int size)
{
	if(size>=3)
	{
		for(int i = 0;i<3;i++)
		{
			_RPT1(0,"z Value %f ", karts[i].getPosition().z);
		}
	}else{
		for(int i = 0;i<size;i++)
		{
			_RPT1(0,"z Value %f ", karts[i].getPosition().z);
		}
	}
	
}