#pragma once
class gameEnemy
{
public:
	gameEnemy();
	~gameEnemy();
	float xPos, yPos, speed;
	int counter;
	bool dirSwitch;
	int ai;
	void enemyPosition(float, float, bool);
	void  virtual moveEnemy();
};

