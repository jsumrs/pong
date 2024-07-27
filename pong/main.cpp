#include <SDL.h>
#include <iostream>
#include <algorithm>

constexpr int SCREEN_WIDTH{ 640 };
constexpr int SCREEN_HEIGHT{ 480 };

SDL_Surface* paddle_surface{ SDL_LoadBMP("img/paddle.bmp") };
SDL_Surface* ball_surface{ SDL_LoadBMP("img/ball.bmp") };


class Player
{
	// Player should be responsible for its own behaviours.
	public:
		const static int movespeed = 10; // Movespeed
		const static int WIDTH = 20;
		const static int HEIGHT = 100;

		Player() : x(0), y(0), x_vel(0), y_vel(0) {}
		Player(int x, int y) : x(x), y(y), x_vel(0), y_vel(0) {}
		Player(int x, int y, int x_vel, int y_vel) : x(x), y(y), x_vel(x_vel), y_vel(y_vel) {}
	
		int getX() { return x; }
		int getY() { return y; }
		void setX(int x) { this->x = x; }
		void setY(int y) { this->y = y; }

		int getX_Vel() { return x_vel; }
		int getY_Vel() { return y_vel; }
		void setX_Vel(int vel) { this->x_vel = vel; }
		void setY_Vel(int vel) { this->y_vel = vel; }

	void updatePlayerPosition()
	{
		setX(std::clamp(x + x_vel, 0, SCREEN_WIDTH - WIDTH));
		setY(std::clamp(y + y_vel, 0, SCREEN_HEIGHT - HEIGHT));
	}

	private:
		int x;
		int y;
		int x_vel;
		int y_vel;

};

class Ball : public Player {
	public: 
		const static int HEIGHT = 20;
		const static int WIDTH = HEIGHT;

		Ball(int x, int y, int x_vel, int y_vel) : Player{ x, y, x_vel, y_vel } {}

		void updatePlayerPosition() 
		{
			setX(std::clamp(this->getX() + this->getX_Vel(), 0, SCREEN_WIDTH - WIDTH));
			setY(std::clamp(this->getY() + this->getY_Vel(), 0, SCREEN_HEIGHT - HEIGHT));
		}
};




void renderPlayer (Player* player, SDL_Renderer* renderer) 
{
	//std::cout << "Rendering player at position: " << player->getX() << ", " << player->getY() << std::endl;

	// Load the paddle image
	SDL_Texture* paddle_texture = SDL_CreateTextureFromSurface(renderer, paddle_surface);
	
	// Set the paddle position
	SDL_Rect paddle_position = { player->getX() , player->getY(), Player::WIDTH, Player::HEIGHT};

	// Draw the paddle
	SDL_RenderCopy(renderer, paddle_texture, nullptr, &paddle_position);
	SDL_DestroyTexture(paddle_texture);
}

void renderBall(Ball* ball, SDL_Renderer* renderer)
{
	//std::cout << "Rendering player at position: " << player->getX() << ", " << player->getY() << std::endl;
	
	// Setting the color key makes the black pixels transparent.
	SDL_SetColorKey(ball_surface, SDL_TRUE, 0x00);

	// Load the paddle image
	SDL_Texture* ball_texture = SDL_CreateTextureFromSurface(renderer, ball_surface);

	// Set the paddle position
	SDL_Rect ball_position = { ball->getX() , ball->getY(), Ball::WIDTH, Ball::HEIGHT};

	// Draw the paddle
	SDL_RenderCopy(renderer, ball_texture, nullptr, &ball_position);
	SDL_DestroyTexture(ball_texture);
}


// Returns the player that is colliding with the ball, null if no collision detected.
Player* checkCollisions(Player* p1, Player* p2, Ball* b) 
{
	// Player 1 Corners
	int p1x = p1->getX();
	int p1x2 = p1x + Player::WIDTH;
	int p1y = p1->getY();
	int p1y2 = p1y + Player::HEIGHT;

	// Player 2 Corners
	int p2x = p2->getX();
	int p2x2 = p2x + Player::WIDTH;
	int p2y = p2->getY();
	int p2y2 = p2y + Player::HEIGHT;

	// Ball Corners
	int bx = b->getX();
	int bx2 = bx + Ball::WIDTH;
	int by = b->getY();
	int by2 = by + Ball::HEIGHT;

	// Check left of ball for collision against wall or paddle
	if (bx <= p1x2 && by >= p1y && by <= p1y2) {
		b->setX_Vel(-b->getX_Vel());
	}
	else if (bx <= 0) {
		// Ball hit left wall
		b->setX_Vel(-b->getX_Vel());
	}
	
	// Check right of ball for collision against wall or paddle
	if (bx2 >= p2x && by >= p2y && by <= p2y2) {
		b->setX_Vel(-b->getX_Vel());
	}
	else if (bx2 >= SCREEN_WIDTH) {
		// Ball hit left wall
		b->setX_Vel(-b->getX_Vel());
	}
	// Check top of ball for collision against wall
	if (by <= 0) {
		b->setY_Vel(-b->getY_Vel());
	}
	else if (by2 >= SCREEN_HEIGHT) {
		b->setY_Vel(-b->getY_Vel());
	}

	// Check bottom of ball for collision against wall
	return NULL;
}

int main ( int argc, char* args[])
{
		SDL_Window* window { NULL };
		SDL_Renderer* renderer { NULL };
		SDL_Surface* screenSurface { NULL };

		int SDL_Init_return_val{ SDL_Init(SDL_INIT_VIDEO) };
		if ( SDL_Init_return_val < 0) 
		{
			std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError();
			return SDL_Init_return_val;
		} else {
			window = SDL_CreateWindow(
										"Pong", 
										SDL_WINDOWPOS_UNDEFINED, 
										SDL_WINDOWPOS_UNDEFINED, 
										SCREEN_WIDTH, 
										SCREEN_HEIGHT, 
										SDL_WINDOW_BORDERLESS
									  );

			if (!window) 
			{
				std::cout << "Failed to create window: " << SDL_GetError();
			}
			else {
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
				if (!renderer) {
					std::cout << "Failed to create renderer: " << SDL_GetError();
				}
				else {
					Player *player1 = new Player(0, (SCREEN_HEIGHT / 2) - 100);
					Player *player2 = new Player(SCREEN_WIDTH - Player::WIDTH, (SCREEN_HEIGHT / 2) - 100);
					Ball* ball = new Ball((SCREEN_WIDTH / 2) - Ball::WIDTH, (SCREEN_HEIGHT / 2) - Ball:: HEIGHT, -5, 5);

					bool running = true;
					while (running) 
					{
						SDL_Event event;
						while (SDL_PollEvent(&event)) 
						{
							std::cout << "Event detected: " << event.type;
							switch (event.type) 
							{
								case SDL_KEYDOWN:
									switch (event.key.keysym.sym)
									{
										case SDLK_ESCAPE: // Quit the program when ESC key is pressed.
											running = false;
											break;
										case SDLK_w:
											player1->setY_Vel(-Player::movespeed);
											break;
										case SDLK_s:
											player1->setY_Vel(Player::movespeed);
											break;
										case SDLK_UP:
											player2->setY_Vel(-Player::movespeed);
											break;
										case SDLK_DOWN:
											player2->setY_Vel(Player::movespeed);
										default:
											break;
									}
									break;

								case SDL_KEYUP:
									switch (event.key.keysym.sym)
									{
										// Check if the other key is pressed before setting the velocity to 0.
									case SDLK_w:
										if (player1->getY_Vel() < 0) player1->setY_Vel(0);
										break;
									case SDLK_s:
										if (player1->getY_Vel() > 0) player1->setY_Vel(0);
										break;
									case SDLK_UP:
										if (player2->getY_Vel() < 0) player2->setY_Vel(0);
										break;
									case SDLK_DOWN:
										if (player2->getY_Vel() > 0) player2->setY_Vel(0);
										break;
									default:
										break;
									}
									break;
								default:
									break;
							}
						}

						// Game updates
						player1->updatePlayerPosition();
						player2->updatePlayerPosition();
						ball->updatePlayerPosition();

						checkCollisions(player1, player2, ball);


						// Rendering
						SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
						SDL_RenderClear(renderer);

						renderPlayer(player1, renderer);
						renderPlayer(player2, renderer);
						renderBall(ball, renderer);

						SDL_RenderPresent(renderer);
						SDL_Delay(10);
					}

					delete player1;
					delete player2;
					delete ball;
				}

			}
		}


		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_FreeSurface(paddle_surface);
		SDL_Quit();

		return 0;
}
