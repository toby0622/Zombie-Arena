#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Zombie {
private:
	// speed for each type of zombie
	const float BLOATER_SPEED = 40;
	const float CHASER_SPEED = 80;
	const float CRAWLER_SPEED = 20;

	// toughness for each type of zombie
	const float BLOATER_HEALTH = 5;
	const float CHASER_HEALTH = 1;
	const float CRAWLER_HEALTH = 3;

	// speed variations (offset)
	const int MAX_VARRIANCE = 30;
	const int OFFSET = 101 - MAX_VARRIANCE;

	// zombie position
	Vector2f m_Position;

	// zombie sprite
	Sprite m_Sprite;

	// zombie crawling speed
	float m_Speed;

	// zombie health
	float m_Health;

	// alive verify
	bool m_Alive;

public:
	// zombie hit by bullet
	bool hit();

	// whether zombie is alive or not
	bool isAlive();

	// spawn a new zombie
	void spawn(float startX, float startY, int type, int seed);

	// return a rectangle that is the position in the world
	FloatRect getPosition();

	// get a copy of the sprite to draw
	Sprite getSprite();

	// update the zombie each frame
	void update(float elapsedTime, Vector2f playerLocation);
};