#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm>

constexpr int SCREEN_WIDTH{ 640 };
constexpr int SCREEN_HEIGHT{ 480 };
constexpr int PLAYER_WIDTH{ 20 };
constexpr int PLAYER_HEIGHT{ 100 };

class Player
{
	// Player should be responsible for its own behaviours.
	public:
		const static int movespeed{ 10 }; // Movespeed

		Player() : x(0), y(0), x_vel(0), y_vel(0) {}
	
		Player(int x, int y) : x(x), y(y), x_vel(0), y_vel(0) {}
	
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
		setX(std::clamp(x + x_vel, 0, SCREEN_WIDTH - PLAYER_WIDTH));
		setY(std::clamp(y + y_vel, 0, SCREEN_HEIGHT - PLAYER_HEIGHT));
	}

	private:
		int x;
		int y;
		int x_vel;
		int y_vel;

};




void renderPlayer (Player* player, SDL_Renderer* renderer) 
{
	//std::cout << "Rendering player at position: " << player->getX() << ", " << player->getY() << std::endl;

	// Load the paddle image
	SDL_Surface* paddleSurface = SDL_LoadBMP( "img/paddle.bmp" );
	SDL_Texture* paddleTexture = SDL_CreateTextureFromSurface(renderer, paddleSurface);
	SDL_FreeSurface(paddleSurface);

	// Set the paddle position
	SDL_Rect paddlePosition = { player->getX() , player->getY(), PLAYER_WIDTH, PLAYER_HEIGHT};

	// Draw the paddle
	SDL_RenderCopy(renderer, paddleTexture, nullptr, &paddlePosition);

	SDL_DestroyTexture(paddleTexture);
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
					Player* player2 = new Player(SCREEN_WIDTH - PLAYER_WIDTH, (SCREEN_HEIGHT / 2) - 100);

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
						player1->updatePlayerPosition();
						player2->updatePlayerPosition();


						// Clear the screen
						SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
						SDL_RenderClear(renderer);

						// Render paddles
						renderPlayer(player1, renderer);
						renderPlayer(player2, renderer);



						// Present the frame
						SDL_RenderPresent(renderer);

						SDL_Delay(10);
					}

					delete player1;
					delete player2;

				}

			}
		}


		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();

		return 0;
}
