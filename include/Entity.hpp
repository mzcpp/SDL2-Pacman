#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL.h>

enum class Direction
{
	LEFT, RIGHT, UP, DOWN, NONE
};

class Game;
class Level;
class Tile;

class Entity
{
private:

protected:
	Game* game_;
	Level* level_;

	Direction direction_;
	Tile* current_tile_;

public:
	Entity(Game* game);
	
	virtual ~Entity();

	void SetLevel(Level* level);
	
	Tile* GetCurrentTile();
	
	Direction GetDirection();

	virtual void Tick() = 0;
	
	virtual void Render() = 0;

	void DebugNeighbors();
};

#endif

// 0 1  2 3