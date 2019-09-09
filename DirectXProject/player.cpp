#include "player.h"

player::player(){
	float COLLISIONDATA = 10.1f;

	m_PlayerX = 0.0f;
	m_PlayerY = 0.0f;

	m_ScreenX = 0.0f;
	m_ScreenY = 0.0f;
	moveLeft = 0;
	moveRight = 0;
	move = 0;
};
player::player(float x, float y)
{
	m_PlayerX = 0.0f;
	m_PlayerY = 0.0f;

	m_ScreenX = 0.0f;
	m_ScreenY = 0.0f;
};

void player::movePlayer(float wallX[], float wallY[], int wallcount)
{
	move = moveLeft + moveRight;
	hSpeed = move*moveSpeed;

	if (vSpeed < 10){
		vSpeed += gravity;
	}
	for (int i = 0; i < wallcount; i++){
		float a = m_PlayerX - wallX[i];
		a = a * a;
		float b = m_PlayerY - wallY[i];
		b = b * b;
		float c = sqrt(a + b);
		if (c < 0.125f) {
			vSpeed = jump * jumpSpeed;
		}
	}
	
	//Horizontal collision
	if (move != 0){
		for (int i = 0; i < wallcount; i++){

			float playerDistance = (m_PlayerX +1.0f /8.0f) - wallX[i];
			playerDistance = sqrt(playerDistance);


			if ((move > 0) && (playerDistance < 0.128f))
			{
				if (((m_PlayerY - (0.5f / 8.0f)) >(wallY[i] - (1.0f / 8.0f))) && ((m_PlayerY - (0.5f / 8.0f)) < wallY[i])){

					hSpeed = 0;			
				}
			}
			playerDistance = (m_PlayerX - 1.0f / 8.0f) - wallX[i];
			playerDistance = sqrt(playerDistance);

			if ((move < 0) && (playerDistance < 0.120f))
			{
				if (((m_PlayerY - (0.5f / 8.0f)) >(wallY[i] - (1.0f / 8.0f))) && ((m_PlayerY - (0.5f / 8.0f)) < wallY[i])){

					hSpeed = 0;
				}
			}
		}
	}
	m_PlayerX += hSpeed;

	//Verticle collision
	for (int i = 0; i < wallcount; i++){
		float playerDistanceY = (m_PlayerY + 1.0f / 8.0f) - wallY[i];
		playerDistanceY = sqrt(playerDistanceY);


		if (playerDistanceY < 0.2f)
		{
			if (((m_PlayerX - (0.5f / 8.0f)) >(wallX[i] - (1.0f / 8.0f))) && ((m_PlayerX - (0.5f / 8.0f)) < wallX[i])){
				vSpeed = -0.01;
			}
		}
	}
		m_PlayerY += vSpeed;

}


float player::getPlayerPosition(char position){
	if ((position == 'x') || (position == 'X')){
		return m_PlayerX;

	}
	else if ((position == 'y') || (position == 'Y')){
		return m_PlayerY;
	}
	else{
		return 1000.00f;
	}
}


void player::setPlayerX(float x){
	m_PlayerX = (-x + 1.0f) / 8.0f;
}
void player::setPlayerY(float y){
	m_PlayerY = (-y + 1.0f) / 8.0f;
}


float player::pixelize(float pixelsWanted){
	float pixels = (pixelsWanted / 8.0f) / 64.0f;
	return pixels;
}