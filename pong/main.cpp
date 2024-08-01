#include <glad/glad.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>

// From GLM Lib
#include <glm.hpp>
#include <vec2.hpp>


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
		score += 1;
		return score;
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




void renderPlayer(Player& player, SDL_Renderer* renderer)
{
	//std::cout << "Rendering player at position: " << player.getX() << ", " << player.getY() << std::endl;

	// Load the paddle image
	SDL_Texture* paddle_texture = SDL_CreateTextureFromSurface(renderer, paddle_surface);

	// Set the paddle position
	SDL_Rect paddle_position = { player.x , player.y, Player::WIDTH, Player::HEIGHT };

	// Draw the paddle
	SDL_RenderCopy(renderer, paddle_texture, nullptr, &paddle_position);
	SDL_DestroyTexture(paddle_texture);
}

void renderBall(Ball& ball, SDL_Renderer* renderer)
{
	//std::cout << "Rendering player at position: " << player.getX() << ", " << player.getY() << std::endl;

	// Setting the color key makes the black pixels transparent.
	SDL_SetColorKey(ball_surface, SDL_TRUE, 0x00);

	// Load the paddle image
	SDL_Texture* ball_texture = SDL_CreateTextureFromSurface(renderer, ball_surface);

	// Set the paddle position
	SDL_Rect ball_position = { ball.x , ball.y, Ball::WIDTH, Ball::HEIGHT };

	// Draw the paddle
	SDL_RenderCopy(renderer, ball_texture, nullptr, &ball_position);
	SDL_DestroyTexture(ball_texture);
}

void renderScore(SDL_Renderer* renderer, Player& p, TTF_Font* font, int x, int y) 
{
	SDL_Color font_color{ 255, 255, 255 };

	// Convert score into a c-style string.
	const int buffer_size = 4;
	char p_score_buffer[buffer_size];
	std::snprintf(p_score_buffer, buffer_size, "%d", p.score);

	SDL_Surface* p_text_surface = TTF_RenderText_Solid(font, p_score_buffer, font_color);
	SDL_Texture* p_text_texture = SDL_CreateTextureFromSurface(renderer, p_text_surface);
	SDL_FreeSurface(p_text_surface);

	SDL_Rect p_text_rect = { x, y, p_text_surface->w, p_text_surface->h, };
	SDL_RenderCopy(renderer, p_text_texture, nullptr, &p_text_rect);

	SDL_DestroyTexture(p_text_texture);
}

bool checkCollisionAgainstPlayer(Player& player, Ball& ball)
{
	// https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection

	glm::vec2 ball_pos{ ball.x, ball.y };
	glm::vec2 ball_center{ ball_pos + Ball::RADIUS };

	glm::vec2 player_half_extents { Player::WIDTH / 2.0f, Player::HEIGHT / 2.0f };
	glm::vec2 player_center { player.x + player_half_extents.x, player.y + player_half_extents.y };

	glm::vec2 difference = ball_center - player_center;
	glm::vec2 clamped { glm::clamp(difference, -player_half_extents, player_half_extents) };
	
	// Closest point on player to the circle.
	glm::vec2 closest = clamped + player_center;
	difference = ball_center - closest;

	return glm::length(difference) < Ball::RADIUS;
}

void handlePlayerBallCollisions(Ball& ball, Player& player)
{
	if (checkCollisionAgainstPlayer(player , ball)) {
		ball.setX_Vel(-ball.x);
	}

}


bool checkCollisionsAgainstNorthWall(Ball& ball)
{
	return ball.y <= 0;
}

bool checkCollisionsAgainstEastWall(Ball& ball)
{
	return ball.x + Ball::WIDTH >= SCREEN_WIDTH;
}

bool checkCollisionsAgainstSouthWall(Ball& ball) 
{
	return ball.y + Ball::HEIGHT >= SCREEN_HEIGHT;
}

bool checkCollisionsAgainstWestWall(Ball& ball)
{
	return ball.x <= 0;
}

char checkCollisionsAgainstWalls(Ball& ball) 
{

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

void handleWallCollisions(Ball& ball, Player& player1, Player& player2) 
{
	switch (checkCollisionsAgainstWalls(ball))
		{
		case 'n': // Ceiling
			ball.setY_Vel(-ball.y);
			break;
		case 's': // Floor
			ball.setY_Vel(-ball.y);
			break;
		case 'e': // P2's Wall
			ball.setX_Vel(-ball.x);
			player1.scorePoint();
			break;
		case 'w': // P1's Wall
			ball.setX_Vel(-ball.x);
			player2.scorePoint();
			break;
		default:
			break;
		}

}



int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError();
		return -1;
	}

	// Use OpenGL 3.3 core.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_Window* window { SDL_CreateWindow (
		"Pong",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL
		) 
	};

	if (!window)
	{
		std::cout << "Failed to create window: " << SDL_GetError();
		return -1;
	}
	
	SDL_GLContext glContext{ SDL_GL_CreateContext(window) };
	
	// Load our GL functions with GLAD.
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to load GLAD: " << SDL_GetError();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::cout << "Failed to create renderer: " << SDL_GetError();
		return -1;
	}
		
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

	// TODO CHANGE THESE TO REFERENCES NOT POINTERS. WHEN DELETING THESE AND NOT MAKING THEM nullptr THEY ARE CONSIDERED "DANGLING" POINTERS
	// WHICH MAY LEAD TO UNDEFINED BEHAVIOUR AND / OR CRASHES. REFERENCES ARE EASIER TO WORK WITH.

	Player p1(0, (SCREEN_HEIGHT / 2) - 100);
	Player p2(SCREEN_WIDTH - Player::WIDTH, (SCREEN_HEIGHT / 2) - 100);
	Ball b((SCREEN_WIDTH / 2) - Ball::WIDTH, (SCREEN_HEIGHT / 2) - Ball::HEIGHT, -5, 5);
	Player& player1 = p1;
	Player& player2 = p2;
	Ball& ball = b;
	unsigned int game_speed = 10;
	bool running = true;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE: // Quit the program when ESC key is pressed.
							running = false;
							break;
						case SDLK_w:
							player1.setY_Vel(-Player::MOVESPEED);
							break;
						case SDLK_s:
							player1.setY_Vel(Player::MOVESPEED);
							break;
						case SDLK_UP:
							player2.setY_Vel(-Player::MOVESPEED);
							break;
						case SDLK_DOWN:
							player2.setY_Vel(Player::MOVESPEED);
							break;
						case SDLK_PERIOD:
							if (game_speed >= 1) {
								game_speed--;
								std::cout << "Gamespeed: " << game_speed << std::endl;
							}
							break;
						case SDLK_COMMA:
							game_speed++;
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
							if (player1.getY_Vel() < 0) player1.setY_Vel(0);
							break;
						case SDLK_s:
							if (player1.getY_Vel() > 0) player1.setY_Vel(0);
							break;
						case SDLK_UP:
							if (player2.getY_Vel() < 0) player2.setY_Vel(0);
							break;
						case SDLK_DOWN:
							if (player2.getY_Vel() > 0) player2.setY_Vel(0);
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
		player1.updatePosition();

		player2.setY_Vel(ball.getY_Vel());
		player2.updatePosition();

		ball.updatePosition();
		
		// Collisions
		handlePlayerBallCollisions(ball, player1);
		handlePlayerBallCollisions(ball, player2);
		handleWallCollisions(ball, player1, player2);


		// Rendering
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);

		renderPlayer(player1, renderer);
		renderPlayer(player2, renderer);
		renderBall(ball, renderer);
		renderScore(renderer, player1, font, 200, 200);
		renderScore(renderer, player2, font, 400, 200);

		SDL_RenderPresent(renderer);
		SDL_Delay(game_speed);
	}


	TTF_CloseFont(font);


	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(paddle_surface);
	SDL_Quit();

	return 0;
}
