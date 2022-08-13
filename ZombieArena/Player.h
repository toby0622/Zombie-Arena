#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Player {
private:
	const float START_SPEED = 200;
	const float START_HEALTH = 100;

	Vector2f m_Position;
	Sprite m_Sprite;
	Texture m_Texture;

	// screen resolution
	Vector2f m_Resolution;

	// size of the arena
	IntRect m_Arena;

	// tile size of the arena
	int m_TileSize;

	// directions the player currently moving
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;

	// health the player got
	int m_Health;

	// maximum health the player can have
	int m_MaxHealth;

	// last time the player be hit
	Time m_LastHit;

	// speed in pixels per second
	float m_Speed;

public:
	Player();
	void spawn(IntRect arena, Vector2f resolution, int tileSize);

	// handle the player getting hit by a zombie
	bool hit(Time timeHit);

	// how long ago was the player last hit
	Time getLastHitTime();

	// where is the player
	FloatRect getPosition();

	// where is the center of the player
	Vector2f getCenter();

	// which angle is the player facing
	float getRotation();

	// send a copy of the sprite to main
	Sprite getSprite();

	// how much health has the player currently got?
	int getHealth();

	// four functions move the player
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	// stop the player moving in a specific direction
	void stopLeft();
	void stopRight();
	void stopUp();
	void stopDown();

	void update(float elapsedTime, Vector2i mousePosition);

	// give the player a speed boost
	void upgradeSpeed();

	// give the player some health
	void upgradeHealth();

	// increase the maximum amount of health the player can have
	void increaseHealthLevel(int amount);
};



