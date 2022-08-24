#pragma once
#include "Vector3.hpp"

class LocalPlayer
{
private:
	LocalPlayer();

public:
	static LocalPlayer* Get();

	int* GetHealth();
	int* GetTeam();
	Vector3* GetOrigin();
	Vector3* GetViewOffset();
	void AimAt(Vector3* target);
	float GetDistance(Vector3*);
};
