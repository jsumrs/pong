#ifndef PONG_H
#define PONG_H

#include "player.h"
#include <SDL.h>
#include <SDL_ttf.h>

// Constants
constexpr int SCREEN_WIDTH{ 1000 };
constexpr int SCREEN_HEIGHT{ 600 };

// External surfaces
extern SDL_Surface* paddle_surface;
extern SDL_Surface* ball_surface;

// Forward declarations for functions used in main.cpp
void renderPlayer(Player& player, SDL_Renderer* renderer);
void renderBall(Ball& ball, SDL_Renderer* renderer);
void renderScore(SDL_Renderer* renderer, Player& p, TTF_Font* font, int x, int y);
bool checkCollisionAgainstPlayer(Player& player, Ball& ball);
void handlePlayerBallCollisions(Ball& ball, Player& player);
bool checkCollisionsAgainstNorthWall(Ball& ball);
bool checkCollisionsAgainstEastWall(Ball& ball);
bool checkCollisionsAgainstSouthWall(Ball& ball);
bool checkCollisionsAgainstWestWall(Ball& ball);
char checkCollisionsAgainstWalls(Ball& ball);
void handleWallCollisions(Ball& ball, Player& player1, Player& player2);
double randomNumber(double min, double max);

#endif // PONG_H