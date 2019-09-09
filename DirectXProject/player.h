#pragma once
#include <math.h>
class player
{
public:
	int moveLeft;
	int moveRight;
	int move;
	bool jump = false;
	float vSpeed = 0.0f, hSpeed = 0.0f, gravity = pixelize(-0.4f), jumpSpeed = pixelize(12.0f), moveSpeed = pixelize(8.0f);
	float pixel = (1.0f / 8.0f) / 64;
	int sign = 0;
	float COLLISIONDATA;
	player();
	player(float x, float y);

	void movePlayer(float[],float[],int);
	float getPlayerPosition(char position);
	void player::setPlayerX(float x);
	void player::setPlayerY(float y);
public:

	float pixelize(float);
	float m_PlayerX, m_PlayerY;
	float m_OldPlayerX, m_OldPlayerY;
	float m_ScreenX, m_ScreenY;
};

