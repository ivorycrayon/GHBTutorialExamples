#pragma once
#include <stdint.h>
#include "Vector3.hpp"

class Player
{
private:
	Player();

public:
	static int* GetMaxPlayers();
	static Player* GetPlayer(int index);

	int* GetHealth();
	int* GetTeam();
	Vector3* GetOrigin();
	Vector3* GetViewOffset();
	Vector3* GetBonePos(int boneID);
};