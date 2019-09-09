#include "gameData.h"

gameData::gameData(int counter[], int enemycount)
{
	enemy			= new gameEnemy[enemycount];
	newPlayer		= new player;
	newSky			= new wall[counter[0]];
	newWall			= new wall[counter[1]];
	newDirtGrass	= new wall[counter[2]];
	newDirt			= new wall[counter[3]];
	newCloud		= new wall[counter[4]];
	newFlower		= new wall[counter[5]];
	newFlowerStem	= new wall[counter[6]];
	newHillLeft		= new wall[counter[7]];
	newHillMiddle	= new wall[counter[8]];
	newHillRight	= new wall[counter[9]];
	newMountLeft	= new wall[counter[10]];
	newMountMiddle	= new wall[counter[11]];
	newMountRight	= new wall[counter[12]];
	newError		= new wall[counter[13]];
	enemyNo = enemycount;

	D3DColourBlack = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

}

gameData::~gameData()
{
	delete		newPlayer;		newPlayer = NULL;
	delete[]	enemy;			enemy = NULL;
	delete[]	newWall;		newWall = NULL;
	delete[]	newFlower;		newFlower = NULL;
	delete[]	newFlowerStem;	newFlowerStem = NULL;
	delete[]	newSky;			newSky = NULL;
	delete[]	newDirt;		newDirt = NULL;
	delete[]	newDirtGrass;	newDirtGrass = NULL;
	delete[]	newCloud;		newCloud = NULL;
	delete[]	newError;		newError = NULL;
	delete[]	newHillLeft;	newHillLeft = NULL;
	delete[]	newHillMiddle;	newHillMiddle = NULL;
	delete[]	newHillRight;	newHillRight = NULL;
	delete[]	newMountLeft;	newMountLeft = NULL;
	delete[]	newMountMiddle;	newMountMiddle = NULL;
	delete[]	newMountRight;	newMountRight = NULL;
}