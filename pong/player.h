#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <algorithm>

class Player
{
	// Player should be responsible for its own behaviours.
public:
	const static int MOVESPEED = 10; // Movespeed
	const static int WIDTH = 20;
	const static int HEIGHT = 100;
	const static SDL_KeyCode DEFAULT_UP_CONTROL = SDL_KeyCode::SDLK_w;
	const static SDL_KeyCode DEFAULT_DOWN_CONTROL = SDL_KeyCode::SDLK_s;
	const SDL_KeyCode up_control;
	const SDL_KeyCode down_control;
	int score = 0;
	int x;
	int y;
	int x_vel;
	int y_vel;

	Player(int x, int y, int x_vel, int y_vel, const SDL_KeyCode up_control, const SDL_KeyCode down_control) : x(x), y(y), x_vel(x_vel), y_vel(y_vel), up_control(up_control), down_control(down_control) {}
	Player(int x, int y, int x_vel, int y_vel) : Player(x, y, x_vel, y_vel, DEFAULT_UP_CONTROL, DEFAULT_DOWN_CONTROL) {}
	Player(int x, int y) : Player(x, y, 0, 0) {}
	Player() : Player(0, 0) {}


	int getX() { return x; }
	int getY() { return y; }
	void setX(int x2) { x = x2; }
	void setY(int y2) { y = y2; }

	int getX_Vel() { return x_vel; }
	int getY_Vel() { return y_vel; }
	void setX_Vel(int vel) { x_vel = vel; }
	void setY_Vel(int vel) { y_vel = vel; }

	int getScore() { return score; }

	void updatePosition()
	{
		setX(std::clamp(x + x_vel, 0, SCREEN_WIDTH - WIDTH));
		setY(std::clamp(y + y_vel, 0, SCREEN_HEIGHT - HEIGHT));
	}

	int scorePoint() {
		return ++score;
	}
};

class Ball : public Player
{
public:
	constexpr static int HEIGHT = 20;
	constexpr static int WIDTH = HEIGHT;
	constexpr static float RADIUS = WIDTH / 2.0f;

	Ball(int x, int y, int x_vel, int y_vel) : Player{ x, y, x_vel, y_vel } {}

	void updatePosition()
	{
		setX(std::clamp(x + x_vel, 0, SCREEN_WIDTH - WIDTH));
		setY(std::clamp(y + y_vel, 0, SCREEN_HEIGHT - HEIGHT));
	}
};

#endif
