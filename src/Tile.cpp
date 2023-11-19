#include "Tile.hpp"
#include "Game.hpp"

#include <SDL2/SDL.h>

Tile::Tile(Game* game) : 
	game_(game), 
	type_(TileType::EMPTY), 
	tile_size_(0), 
	pellet_(false), 
	pellet_spawned_(false), 
	energizer_(false), 
	energizer_spawned_(false)
{
	rect_.x = 0;
	rect_.y = 0;
	rect_.w = 0;
	rect_.h = 0;
}

Tile::~Tile()
{
}

void Tile::Tick()
{
}

void Tile::Render() const
{
	if (type_ == TileType::GHOST_GATE)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0xff, 0xaf, 0xb9, 0xff);
	}
	else if (type_ == TileType::GHOST_HOME)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0x00, 0x00, 0x00, 0xff);
	}
	else if (type_ == TileType::WALL)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0x00, 0x00, 0xaa, 0xff);
	}
	else if (type_ == TileType::PATH)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0x00, 0x00, 0x00, 0xff);
	}
	else
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0x00, 0x00, 0x00, 0xff);
	}

	SDL_RenderFillRect(game_->renderer_, &rect_);

	if (pellet_spawned_ || energizer_spawned_)
	{
		int pellet_size = pellet_ ? 6 : 18;

		SDL_Rect pellet;
		pellet.x = rect_.x + (tile_size_ / 2) - (pellet_size / 2);
		pellet.y = rect_.y + (tile_size_ / 2) - (pellet_size / 2);
		pellet.w = pellet_size;
		pellet.h = pellet_size;
		
		SDL_SetRenderDrawColor(game_->renderer_, 0xff, 0xaf, 0xb9, 0xff);
		SDL_RenderFillRect(game_->renderer_, &pellet);
	}
}

bool Tile::IsWall() const
{
	return type_ == TileType::WALL;
}