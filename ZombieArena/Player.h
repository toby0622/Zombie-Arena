#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Player {
private:
	const float START_SPEED = 200;
	const float START_HEALTH = 100;

	// player position
	Vector2f m_Position;

	// sprite
	Sprite m_Sprite;

	// texture
	Texture m_Texture;

	// screen resolution
	Vector2f m_Resolution;

	// size of the current arena
	IntRect m_Arena;

	// tile of the arena
	int m_TileSize;

	// player's moving direction
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;

	// current health
	int m_Health;
	// maximum health
	int m_MaxHealth;

	// last time receiving hit from zombie
	Time m_LastHit;

	// speed in pixel per second
	float m_Speed;

public:
	Player();
	void spawn(IntRect arena, Vector2f resolution, int tileSize);

	// reset stats (end of every game)
	void resetPlayerStats();

	// handle the player getting hit by the zombie
	bool hit(Time timeHit);

	// how long ago was the player last hit
	Time getLastHitTime();

	// player's position
	FloatRect getPosition();

	// the center of the player
	Vector2f getCenter();

	// the angle that player is facing
	float getRotation();

	// send a copy of the sprite to main function
	Sprite getSprite();

	// player movement
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	// stop the player's movement
	void stopLeft();
	void stopRight();
	void stopUp();
	void stopDown();

	// frame update
	void update(float elapsedTime, Vector2i mousePosition);

	// give the player a speed boost
	void upgradeSpeed();

	// give the player some health
	void upgradeHealth();

	// increase the maximum amount of health
	void increaseHealthLevel(int amount);

	// player's current health
	int getHealth();
};