#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "Tile.hpp"

#include <SDL2/SDL.h>

#include <vector>

class Game;

class Level
{
private:
	Game* game_;
	SDL_Surface* surface_pixels_;
	Uint32* pixels_;

	std::vector<Tile> board_;
	int pixel_width_;
	int pixel_height_;
	int pixel_count_;
	int tile_size_;

public:
	int pellet_count_;
	int energizer_count_;

	Level(Game* game);

	~Level();

	void HandleEvents();
	
	void Tick();
	
	void Render();

	bool Load(const char* path);

	void Initialize(const char* path);

	void Free();

	void Reset();

	int TileDistance(const Tile& source, const Tile& target);

	Tile* GetTile(int x, int y);
	
	Tile* GetUpperTile(int x, int y);
	
	Tile* GetLowerTile(int x, int y);
	
	Tile* GetLeftTile(int x, int y);
	
	Tile* GetRightTile(int x, int y);

	std::vector<Tile*> GetNeighborTiles(int x, int y);

	int GetPixelWidth();
	
	int GetPixelHeight();
	
	int GetPixelCount();

	SDL_Surface* GetPixelSurface();

	Uint32* GetPixels32();

	Uint32 GetPitch32();
};

#endif
