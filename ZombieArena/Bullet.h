#pragma once
#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Bullet {
private:
	// bullet position
	Vector2f m_Position;

	// bullet shape
	RectangleShape m_BulletShape;

	// whether bullet is flying or not
	bool m_InFlight = false;

	// bullet traveling speed
	float m_BulletSpeed = 1000;

	// trigonal distance
	float m_BulletDistanceX;
	float m_BulletDistanceY;

	// boundaries limit the bullet that won't fly forever
	float m_MaxX;
	float m_MinX;
	float m_MaxY;
	float m_MinY;

public:
	// constructor
	Bullet();

	// stop the bullet
	void stop();

	// returns the value of m_InFlight
	bool isInFlight();

	// launch a new bullet
	void shoot(float startX, float startY, float xTarget, float yTarget);

	// return the bullet position
	FloatRect getPosition();

	// return the actual shape (for drawing)
	RectangleShape getShape();

	// update the bullet each frame
	void update(float elapsedTime);
};