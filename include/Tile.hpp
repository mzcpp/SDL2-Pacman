#ifndef TILE_HPP
#define TILE_HPP

#include <SDL2/SDL.h>

enum class TileType
{
	EMPTY, WALL, PATH, GHOST_GATE, GHOST_HOME, GHOST_CROSSROAD
};

class Game;

class Tile
{
private:
	Game* game_;

public:
	TileType type_;
	SDL_Rect rect_;
	int tile_size_;

	bool pellet_;
	bool pellet_spawned_;
	bool energizer_;
	bool energizer_spawned_;

	Tile(Game* game);

	~Tile();

	void Tick();

	void Render() const;

	bool IsWall() const;
};

#endif