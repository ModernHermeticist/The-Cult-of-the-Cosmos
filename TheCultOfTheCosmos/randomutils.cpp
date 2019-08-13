#include "pch.h"
#include "main.h"

int randomChanceByFloor(int percentChance, int curFloor, int firstSeenFloor, float weight, bool incremental)
{
	if (curFloor < firstSeenFloor)
	{
		return 0;
	}
	if (incremental)
	{
		return int(percentChance + (curFloor * weight));
	}
	else
	{
		return int(percentChance - (curFloor * weight));
	}
}
