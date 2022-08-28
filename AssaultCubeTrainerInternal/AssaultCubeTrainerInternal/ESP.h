#pragma once
#include "assaultcube.h"
#include "glDraw.h"
#include "glText.h"

//scaling
const int VIRTUAL_SCREEN_WIDTH = 1024;
const int GAME_UNIT_MAGIC = 400; //differs in every game

const float PLAYER_HEIGHT = 5.25f; //differs in every game
const float PLAYER_WIDTH = 2.0f; //differs in every game
const float EYE_HEIGHT = 4.25f; //differs in every game

const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;

const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;

class ESP
{
public:
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	int* gameMode = (int*)(0x50F49C);
	int* numOfPlayers = (int*)(0x50F500);
	float* matrix = (float*)(0x501AE8);
	PlayerEntity* localPlayer = *(PlayerEntity**)(moduleBase + 0x10F4F4);
	EntList* entList = *(EntList**)(moduleBase + 0x10F4F8);

	int viewport[4];

	bool IsTeamGame();
	bool IsEnemy(PlayerEntity* e);
	bool isValidEnt(PlayerEntity* ent);

	void DrawESPBox(PlayerEntity* p, Vec3, GL::Font& font);
	void Draw(GL::Font& font);
};