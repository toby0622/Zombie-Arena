#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <fstream>
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

	// about the game
	int score = 0;
	int hiScore = 0;

	// for the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);

	// load the font
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");

	// "PAUSED"
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \nto continue");

	// "GAME OVER"
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");

	// "LEVELING UP"
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(150, 250);

	std::stringstream levelUpStream;

	levelUpStream << "1- Increased rate of fire" << "\n2- Increased clip size(next reload)"
		<< "\n3- Increased max health" << "\n4- Increased run speed"
		<< "\n5- More and better health pickups" << "\n6- More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	// ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(200, 980);

	// score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 0);

	// load the high score from a text file
	std::ifstream inputFile("gamedata/scores.txt");

	if (inputFile.is_open()) {
		// ">>" reads the data
		inputFile >> hiScore;
		inputFile.close();
	}

	// hi score
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "Hi Score:" << hiScore;
	hiScoreText.setString(s.str());

	// zombies remaining
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(55);
	zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(1500, 980);
	zombiesRemainingText.setString("Zombies: 100");

	// wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	// health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);

	// last time the HUD be updated
	int framesSinceLastHUDUpdate = 0;

	// HUD update interval
	int fpsMeasurementFrameInterval = 1000;

	// setup the hit sound
	SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("sound/hit.wav");
	Sound hit;
	hit.setBuffer(hitBuffer);

	// setup the splat sound
	SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("sound/splat.wav");
	Sound splat;
	splat.setBuffer(splatBuffer);

	// setup the shoot sound
	SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("sound/shoot.wav");
	Sound shoot;
	shoot.setBuffer(shootBuffer);

	// setup the reload sound
	SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("sound/reload.wav");
	Sound reload;
	reload.setBuffer(reloadBuffer);

	// setup the failed sound
	SoundBuffer reloadFailedBuffer;
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);

	// setup the powerup sound
	SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile("sound/powerup.wav");
	Sound powerup;
	powerup.setBuffer(powerupBuffer);

	// setup the pickup sound
	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("sound/pickup.wav");
	Sound pickup;
	pickup.setBuffer(pickupBuffer);

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
				// increase fire rate
				fireRate++;
				
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num2) {
				// increase clip size
				clipSize += clipSize;
				
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num3) {
				// increase health
				player.upgradeHealth();
				
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

			// collision detection
			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < numZombies; j++) {
					if (bullets[i].isInFlight() && zombies[j].isAlive()) {
						if (bullets[i].getPosition().intersects(zombies[j].getPosition())) {
							// stop the bullet
							bullets[i].stop();

							// register the hit and see if it was a kill
							if (zombies[j].hit()) {
								// bullet hit and zombie killed
								score += 10;
								
								if (score >= hiScore) {
									hiScore = score;
								}

								numZombiesAlive--;

								// when all the zombies are dead (again)
								if (numZombiesAlive == 0) {
									state = State::LEVELING_UP;
								}
							}

						}
					}
				}
			}// end of zombie being shot

			// whether zombie touches the player or not
			for (int i = 0; i < numZombies; i++) {
				if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive()) {
					if (player.hit(gameTimeTotal)) {
						// More here later
					}

					if (player.getHealth() <= 0) {
						state = State::GAME_OVER;

						std::ofstream outputFile("gamedata/scores.txt");

						// "<<" writes the data
						outputFile << hiScore;
						outputFile.close();
					}
				}
			}// end of player being touched

			// whether the player has touches health pickup
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned()) {
				player.increaseHealthLevel(healthPickup.gotIt());
			}

			// whether the player has touches ammo pickup
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned()) {
				bulletsSpare += ammoPickup.gotIt();
			}

			// size up the health bar
			healthBar.setSize(Vector2f(player.getHealth() * 3, 50));

			// increment the number of frames since the previous update
			framesSinceLastHUDUpdate++;

			// re-calculate every "fpsMeasurementFrameInterval" frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval) {
				// update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombiesAlive;

				// update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());

				// update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// update the high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// update the high score text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());
				framesSinceLastHUDUpdate = 0;
			} // end of HUD update
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

			// switch to the HUD view
			window.setView(hudView);

			// draw all the HUD elements
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (state == State::LEVELING_UP) {
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED) {
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER) {
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}

		window.display();
	} // end of the game loop

	// delete the previously allocated memory (if any)
	delete[] zombies;

	return 0;
}