#include "enemy.h"

gameEnemy::gameEnemy()
{
	xPos = (-13.0f + 1.0f) / 8.0f;
	yPos = (-18.0f +1.0f) /8.0f;
	ai = 1;
	counter = 0;
	dirSwitch = true;
	speed = 0.01f;
}

gameEnemy::~gameEnemy()
{
}


void gameEnemy::moveEnemy(){
	if (ai == 0){

	}
	else if (ai == 1){
		
		if (dirSwitch == false){
			xPos += speed;
			counter += 1;
		}
		else if(dirSwitch == true){
			xPos -= speed;
			counter += 1;

		}
		if (counter > 150){
			counter = 0;
			if (dirSwitch == true){
				dirSwitch = false;
			}
			else if (dirSwitch == false){
				dirSwitch = true;
			}

		}
	}
}
void gameEnemy::enemyPosition(float x, float y, bool dir){
	xPos = (-x + 1.0f) / 8.0f;
	yPos = (-y + 1.0f) / 8.0f;
	dirSwitch = dir;
}