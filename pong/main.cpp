#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>


constexpr int SCREEN_WIDTH{ 640 };
constexpr int SCREEN_HEIGHT{ 480 };

SDL_Surface* paddle_surface{ SDL_LoadBMP("resources/paddle.bmp") };
SDL_Surface* ball_surface{ SDL_LoadBMP("resources/ball.bmp") };


class Player
{
	// Player should be responsible for its own behaviours.
	public:
		const static int MOVESPEED = 10; // Movespeed
		const static int WIDTH = 20;
		const static int HEIGHT = 100;
		int score = 0;

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

		int getScore() { return score; }

	void updatePlayerPosition()
	{
		setX(std::clamp(x + x_vel, 0, SCREEN_WIDTH - WIDTH));
		setY(std::clamp(y + y_vel, 0, SCREEN_HEIGHT - HEIGHT));
	}

	int scorePoint() {
		score += 1;
		return score;
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
		const static int RADIUS = WIDTH / 2;
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

void renderScore(SDL_Renderer* renderer, Player* p, TTF_Font* font, int x, int y) {
	SDL_Color font_color{ 255, 255, 255 };
	
	const int buffer_size = 4;
	char p_score_buffer[buffer_size];
	std::snprintf(p_score_buffer, buffer_size, "%d", p->getScore());
	SDL_Surface* p_text_surface = TTF_RenderText_Solid(font, p_score_buffer, font_color);
	
	SDL_Texture* p_text_texture = SDL_CreateTextureFromSurface(renderer, p_text_surface);
	SDL_FreeSurface(p_text_surface);

	SDL_Rect p_text_rect = { x, y, p_text_surface->w, p_text_surface->h, };
	SDL_RenderCopy(renderer, p_text_texture, nullptr, &p_text_rect);

	SDL_DestroyTexture(p_text_texture);
}

bool checkCollisionsAgainstPlayer(Player* player, Ball* ball)
{
	// Player axes
	int player_x = player->getX();
	int player_x2 = player_x + Player::WIDTH;
	int player_y = player->getY();
	int player_y2 = player_y + Player::HEIGHT;

	// Ball axes  
	int ball_x = ball->getX();
	int ball_x2 = ball_x + Ball::WIDTH;
	int ball_y = ball->getY();
	int ball_y2 = ball_y + Ball::HEIGHT;
    
	bool collision_x = player_x2 >= ball_x && ball_x2 >= player_x;
	bool collision_y = player_y2 >= ball_y && ball_y2 >= player_y;

	return collision_x && collision_y;
}


bool checkCollisionsAgainstNorthWall(Ball* ball) {
	return ball->getY() <= 0;
}

bool checkCollisionsAgainstEastWall(Ball* ball) {
	return ball->getX() + Ball::WIDTH >= SCREEN_WIDTH;
}

bool checkCollisionsAgainstSouthWall(Ball* ball) {
	return ball->getY() + Ball::HEIGHT >= SCREEN_HEIGHT;
}

bool checkCollisionsAgainstWestWall(Ball* ball) {
	return ball->getX() <= 0;
}

char checkCollisionsAgainstWalls(Ball* ball) {

	if (checkCollisionsAgainstNorthWall(ball)) {
		return 'n';
	}
	else if (checkCollisionsAgainstEastWall(ball)) {
		return 'e';
	}
	else if (checkCollisionsAgainstSouthWall(ball)) {
		return 's';
	}
	else if (checkCollisionsAgainstWestWall(ball)) {
		return 'w';
	}
	else {
		return '\0';
	}
}



int main ( int argc, char* args[])
{
		SDL_Window* window { NULL };
		SDL_Renderer* renderer { NULL };
		SDL_Surface* screenSurface { NULL };

		int game_speed = 10;

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
				return -1;
			}
			else {
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
				if (!renderer) {
					std::cout << "Failed to create renderer: " << SDL_GetError();
					return -1;
				}
				else {
					// Init sdl font library
					if (TTF_Init() == -1) {
						std::cout << "Error loading font library: " << TTF_GetError();
						return -1;
					}
					// Load our font.
					TTF_Font* font = TTF_OpenFont("resources/NotoSansMono-Thin.ttf", 62);
					if (font == nullptr) {
						std::cout << "Error loading font: " << TTF_GetError();
						return -1;
					}


					Player *player1 = new Player(0, (SCREEN_HEIGHT / 2) - 100);
					Player *player2 = new Player(SCREEN_WIDTH - Player::WIDTH, (SCREEN_HEIGHT / 2) - 100);
					Ball* ball = new Ball((SCREEN_WIDTH / 2) - Ball::WIDTH, (SCREEN_HEIGHT / 2) - Ball:: HEIGHT, -5, 5);

					bool running = true;
					while (running) 
					{
						SDL_Event event;
						while (SDL_PollEvent(&event)) 
						{
							//std::cout << "Event detected: " << event.type;

							switch (event.type) 
							{
								case SDL_KEYDOWN:
									switch (event.key.keysym.sym)
									{
										case SDLK_ESCAPE: // Quit the program when ESC key is pressed.
											running = false;
											break;
										case SDLK_w:
											player1->setY_Vel(-Player::MOVESPEED);
											break;
										case SDLK_s:
											player1->setY_Vel(Player::MOVESPEED);
											break;
										case SDLK_UP:
											player2->setY_Vel(-Player::MOVESPEED);
											break;
										case SDLK_DOWN:
											player2->setY_Vel(Player::MOVESPEED);
											break;
										case SDLK_PERIOD:
											if (game_speed >= 1) {
												game_speed -= 1;
												std::cout << "Gamespeed: " << game_speed << std::endl;
											}
											break;
										case SDLK_COMMA:
											game_speed += 1;
											std::cout << "Gamespeed: " << game_speed << std::endl;
											break;
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

						player2->setY_Vel(ball->getY_Vel());
						player2->updatePlayerPosition();
						
						
						ball->updatePlayerPosition();


						if (checkCollisionsAgainstPlayer(player1, ball)) {
							ball->setX_Vel(-ball->getX_Vel());
						}

						if (checkCollisionsAgainstPlayer(player2, ball)) {
							ball->setX_Vel(-ball->getX_Vel());
						}
						
						switch (checkCollisionsAgainstWalls(ball))
						{
							case 'n': // Ceiling
								ball->setY_Vel(-ball->getY_Vel());
								break;
							case 's': // Floor
								ball->setY_Vel(-ball->getY_Vel());
								break;
							case 'e': // P2's Wall
								ball->setX_Vel(-ball->getX_Vel());
								player1->scorePoint();
								break;
							case 'w': // P1's Wall
								ball->setX_Vel(-ball->getX_Vel());
								player2->scorePoint();
								break;
							default:
								break;
						}


						// Rendering
						SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
						SDL_RenderClear(renderer);

						renderPlayer(player1, renderer);
						renderPlayer(player2, renderer);
						renderBall(ball, renderer);
						renderScore(renderer, player1, font, 200, 200);
						renderScore(renderer, player2, font, 400, 200);

						SDL_RenderPresent(renderer);
						SDL_Delay(game_speed);
					}

					delete player1;
					delete player2;
					delete ball;
					delete font;
				}

			}
		}


		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_FreeSurface(paddle_surface);
		SDL_Quit();

		return 0;
}
