#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Pickup {
private:
	// start value for health pickups
	const int HEALTH_START_VALUE = 50;
	const int AMMO_START_VALUE = 12;
	const int START_WAIT_TIME = 10;
	const int START_SECONDS_TO_LIVE = 5;

	// sprite that represents the pickup
	Sprite m_Sprite;

	// the arena it exists
	IntRect m_Arena;

	// the amount of pickup's attribute
	int m_Value;

	// type of pickups (1 = health, 2 = ammo)
	int m_Type;

	// handle spawning and disappearing
	bool m_Spawned;
	float m_SecondsSinceSpawn;
	float m_SecondsSinceDeSpawn;
	float m_SecondsToLive;
	float m_SecondsToWait;

public:
	// constructor
	Pickup(int type);

	// prepare a new pickup
	void setArena(IntRect arena);
	void spawn();

	// check the position of a pickup
	FloatRect getPosition();

	// get the sprite for drawing
	Sprite getSprite();

	// the pickup update itself each frame
	void update(float elapsedTime);

	// whether the pickup is currently spawned
	bool isSpawned();

	// claim the pickup ability
	int gotIt();

	// upgrade the value of each pickup
	void upgrade();
};