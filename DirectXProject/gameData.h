#ifndef _GAMEDATA_H
#define _GAMEDATA_H
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include "player.h"
#include "enemy.h"
#include <MMSystem.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma endregion

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// various buffer structs
struct VERTEX { FLOAT X, Y, Z; D3DXVECTOR3 Normal; FLOAT U, V; };

// a struct to define the constant buffer
struct CBUFFER
{
	D3DXMATRIX Final;
	D3DXMATRIX Rotation;
	D3DXVECTOR4 LightVector;
	D3DXCOLOR LightColor;
	D3DXCOLOR AmbientColor;
};
struct wall
{	float wallX;	float wallY;	};

class gameData
{
public:
	gameEnemy * enemy;
	player * newPlayer;
	wall * newWall;
	wall * newSky;
	wall * newCloud;
	wall * newDirt;
	wall * newDirtGrass;
	wall * newError;
	wall * newFlower;
	wall * newFlowerStem;
	wall * newHillLeft;
	wall * newHillMiddle;
	wall * newHillRight;
	wall * newMountLeft;
	wall * newMountMiddle;
	wall * newMountRight;

	D3DXCOLOR D3DColourBlack;

	bool restart = false;
	int enemyNo;
	float cameraX=-1.6f, cameraY=-1.6f;
	int skyCount = 0, grassCount = 0, dirtGrassCount = 0, dirtCount = 0, cloudCount = 0, errorCount = 0, flowerCount = 0, flowerStemCount = 0,
		hillLeftCount = 0, hillRightCount = 0, hillMiddleCount = 0, MountLeftCount = 0, MountRightCount = 0, MountMiddleCount = 0;
	int counter = 0, flowerAniCounter = 0;
	gameData(int[], int);
	~gameData();

};

#endif