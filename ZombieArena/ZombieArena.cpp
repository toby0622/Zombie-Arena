#include <SFML/Graphics.hpp>
#include "ZombieArena.h"
#include "Player.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
using namespace sf;

int main() {
	// instance of "TextureHolder"
	TextureHolder holder;

	// four game execution states
	enum class State {
		PAUSED, LEVELING_UP, GAME_OVER, PLAYING
	};

	// start with the "GAME_OVER" state
	State state = State::GAME_OVER;

	// get the screen resolution and create an execution window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

	// create a an SFML "View" for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// clock for timing everything
	Clock clock;

	// the time that "PLAYING" state been active
	Time gameTimeTotal;

	// mouse in relation to world coordinates
	Vector2f mouseWorldPosition;

	// mouse in relation to screen coordinates
	Vector2i mouseScreenPosition;

	// create an instance of the "Player" class
	Player player;

	// boundaries of the arena
	IntRect arena;

	// create the background
	VertexArray background;

	// load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

	// prepare for a horde of zombies
	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;

	// 100 bullets should do
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;

	// the time that the fire button last pressed
	Time lastPressed;

	// hide the mouse pointer and replace it with crosshair (十字準心)
	window.setMouseCursorVisible(true);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	// create two types of pickups
	Pickup healthPickup(1);
	Pickup ammoPickup(2);

	// main game loop
	while (window.isOpen()) {
		/*
			handle input
		*/

		// handle events
		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::KeyPressed) {
				// pause the game while playing
				if (event.key.code == Keyboard::Return && state == State::PLAYING) {
					state = State::PAUSED;
				}

				// resume from paused
				else if (event.key.code == Keyboard::Return && state == State::PAUSED) {
					state = State::PLAYING;

					// reset the clock to avoid frame jump
					clock.restart();
				}

				// start a new game when "GAME_OVER" state achieved
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER) {
					state = State::LEVELING_UP;
				}

				if (state == State::PLAYING) {
					// reloading
					if (event.key.code == Keyboard::R) {
						if (bulletsSpare >= clipSize) {
							// plenty of bullets
							bulletsInClip = clipSize;
							bulletsSpare -= clipSize;
						} else if (bulletsSpare > 0) {
							// only few bullets left
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
						} else {
							// More here soon?!
						}
					}
				}
			}
		} // end of event polling

		// game quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		// handle controls while playing
		if (state == State::PLAYING) {
			// handle "WASD" key commands
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				player.moveUp();
			} else {
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::S)) {
				player.moveDown();
			} else {
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::A)) {
				player.moveLeft();
			} else {
				player.stopLeft();
			}

			if (Keyboard::isKeyPressed(Keyboard::D)) {
				player.moveRight();
			} else {
				player.stopRight();
			}

			// fire a bullet
			if (Mouse::isButtonPressed(sf::Mouse::Left)) {
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0) {
					// pass the centre of the player and the centre of the cross-hair to the shoot function
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
					currentBullet++;

					if (currentBullet > 99) {
						currentBullet = 0;
					}

					lastPressed = gameTimeTotal;
					bulletsInClip--;
				}
			}// end of bullet firing
		} // end of "WASD" key commands

		// handle "LEVELING_UP" state
		if (state == State::LEVELING_UP) {
			// handle the leveling up abilities
			if (event.key.code == Keyboard::Num1) {
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num2) {
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num3) {
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num4) {
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num5) {
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num6) {
				state = State::PLAYING;
			}

			if (state == State::PLAYING) {
				// level preparation
				arena.width = 500;
				arena.height = 500;
				arena.left = 0;
				arena.top = 0;

				// pass the vertex array by reference to the "createBackground" function
				int tileSize = createBackground(background, arena);

				// spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				// configure the pickups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);

				// create a horde of zombies
				numZombies = 10;

				// delete the previously allocated memory (if any)
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;

				// reset the clock to avoid frame jump
				clock.restart();
			}
		} // end of "LEVELING_UP" handling

		/*
			update the frame
		*/

		if (state == State::PLAYING) {
			// update the delta time
			Time dt = clock.restart();

			// update the total game time
			gameTimeTotal += dt;

			// make a decimal fraction of 1 from the delta time
			float dtAsSeconds = dt.asSeconds();

			// mouse pointing
			mouseScreenPosition = Mouse::getPosition();

			// convert mouse position to world coordinates of "mainView"
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			// set the crosshair to the mouse location
			spriteCrosshair.setPosition(mouseWorldPosition);

			// update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			// make a note of the players new position
			Vector2f playerPosition(player.getCenter());

			// make the view centre around the player				
			mainView.setCenter(player.getCenter());

			// loop through each Zombie and update them
			for (int i = 0; i < numZombies; i++) {
				if (zombies[i].isAlive()) {
					zombies[i].update(dt.asSeconds(), playerPosition);
				}
			}

			// update any bullets that are in-flight
			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					bullets[i].update(dtAsSeconds);
				}
			}

			// update the pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);
		} // end of updating the scene

		/*
			draw the scene
		*/

		if (state == State::PLAYING) {
			window.clear();

			// set the "mainView" to be displayed in the window and draw everything related to it
			window.setView(mainView);

			// draw the background
			window.draw(background, &textureBackground);

			// draw the zombies
			for (int i = 0; i < numZombies; i++) {
				window.draw(zombies[i].getSprite());
			}

			for (int i = 0; i < 100; i++) {
				if (bullets[i].isInFlight()) {
					window.draw(bullets[i].getShape());
				}
			}

			// draw the player
			window.draw(player.getSprite());

			// draw the pickups if currently spawned
			if (ammoPickup.isSpawned()) {
				window.draw(ammoPickup.getSprite());
			}

			if (healthPickup.isSpawned()) {
				window.draw(healthPickup.getSprite());
			}

			// draw the crosshair
			window.draw(spriteCrosshair);
		}

		if (state == State::LEVELING_UP)
		{
		}

		if (state == State::PAUSED)
		{
		}

		if (state == State::GAME_OVER)
		{
		}

		window.display();
	} // end of the game loop

	// delete the previously allocated memory (if any)
	delete[] zombies;

	return 0;
}