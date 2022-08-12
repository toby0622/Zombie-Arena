#include <SFML/Graphics.hpp>
#include "Player.h"
using namespace sf;

int main() {
	// four in-game states
	enum class State {
		PAUSED, LEVELING_UP, GAME_OVER, PLAYING
	};

	// start with "GAME_OVER" state
	State state = State::GAME_OVER;

	// get the screen resolution and create a game window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

	// create a "SFML View" for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// timing clock
	Clock clock;

	// how long the "PLAYING" state is active
	Time gameTimeTotal;

	// mouse in relation to world coordinates
	Vector2f mouseWorldPosition;

	// mouse in relation to screen coordinates
	Vector2i mouseScreenPosition;

	// create an instance of Player class
	Player player;

	// boundaries of the arena
	IntRect arena;

	// main game loop
	while (window.isOpen()) {
		/*
			handle input
		*/

		// handle events by polling
		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::KeyPressed) {
				// pause the game while playing
				if (event.key.code == Keyboard::Return && state == State::PLAYING) {
					state = State::PAUSED;
				}

				// resume while paused
				else if (event.key.code == Keyboard::Return && state == State::PAUSED) {
					state = State::PLAYING;
					
					// reset the clock to avoid frame jump
					clock.restart();
				}

				// start a new game in the "GAME_OVER" state
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER) {
					state = State::LEVELING_UP;
				}

				if (state == State::PLAYING) {

				}
			}
		} // end of event polling

		// handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		// handle "WASD" while playing
		if (state == State::PLAYING) {
			// handle the pressing and releasing of the WASD keys
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
		} // end of "WASD" movement while playing

		// handle the "LEVELING UP" state
		if (state == State::LEVELING_UP) {
			// handle the player "LEVELING UP"
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
				// prepare the level
				arena.width = 500;
				arena.height = 500;
				arena.left = 0;
				arena.top = 0;

				int tileSize = 50;

				// spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				// reset the clock to avoid frame jump
				clock.restart();
			}
		} // end "LEVELING UP" state

		/*
			update the frame
		*/
		if (state == State::PLAYING) {
			// update the delta time
			Time deltaTime = clock.restart();

			// update the total game time
			gameTimeTotal += deltaTime;

			// make a decimal fraction of 1 from the delta time
			float dtAsSeconds = deltaTime.asSeconds();

			// where is the mouse pointer
			mouseScreenPosition = Mouse::getPosition();

			// convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			// update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			// make a note of the players new position
			Vector2f playerPosition(player.getCenter());

			// make the view centre around the player
			mainView.setCenter(player.getCenter());
		} // end updating the scene
	} // end of game loop

	return 0;
}