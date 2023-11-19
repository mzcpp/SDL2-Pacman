#include "Level.hpp"
#include "Game.hpp"
#include "Tile.hpp"
#include "Constants.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <vector>
#include <cmath>

Level::Level(Game* game) : 
	game_(game), 
	surface_pixels_(nullptr), 
	pixels_(nullptr), 
	pixel_width_(0), 
	pixel_height_(0), 
	pixel_count_(0), 
	tile_size_(32), 
	pellet_count_(0), 
	energizer_count_(0)
{	
}

Level::~Level()
{
	Free();
}

void Level::HandleEvents()
{

}
	
void Level::Tick()
{
	const int pellets_on_board = std::count_if(board_.begin(), board_.end(), [](const Tile& tile)
	{
		return tile.pellet_spawned_;
	});

	const int energizers_on_board = std::count_if(board_.begin(), board_.end(), [](const Tile& tile)
	{
		return tile.energizer_spawned_;
	});

	if (pellets_on_board == 0 && energizers_on_board == 0)
	{
		game_->level_completed_ = true;
	}
}

void Level::Render()
{
	std::for_each(board_.begin(), board_.end(), [](const Tile& tile)
	{
		tile.Render();
	});
}

bool Level::Load(const char* path)
{
	Free();

	surface_pixels_ = IMG_Load(path);

	if (surface_pixels_ == nullptr)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
		return false;
	}

	surface_pixels_ = SDL_ConvertSurfaceFormat(surface_pixels_, SDL_GetWindowPixelFormat(game_->window_), 0);

	pixel_width_ = surface_pixels_->w;
	pixel_height_ = surface_pixels_->h;
	pixel_count_ = pixel_width_ * pixel_height_;

	return true;
}

void Level::Initialize(const char* path)
{
	Load(path);

	board_.resize(GetPixelCount(), game_);

	const Uint32 empty_color = SDL_MapRGBA(surface_pixels_->format, 0x00, 0x00, 0x00, 0xff);
	const Uint32 ghost_gate_color = SDL_MapRGBA(surface_pixels_->format, 0xff, 0x64, 0x00, 0xff);
	const Uint32 ghost_home_color = SDL_MapRGBA(surface_pixels_->format, 0xff, 0xff, 0xff, 0xff);
	const Uint32 wall_color = SDL_MapRGBA(surface_pixels_->format, 0x00, 0x00, 0xaa, 0xff);
	const Uint32 energizer_color = SDL_MapRGBA(surface_pixels_->format, 0xff, 0x00, 0x00, 0xff);
	const Uint32 pellet_color = SDL_MapRGBA(surface_pixels_->format, 0xff, 0xaf, 0xb9, 0xff);
	const Uint32 empty_path_color = SDL_MapRGBA(surface_pixels_->format, 0x64, 0x64, 0x64, 0xff);
	const Uint32 ghost_crossroad_pellet_color = SDL_MapRGBA(surface_pixels_->format, 0x00, 0xff, 0xff, 0xff);
	const Uint32 ghost_crossroad_empty_color = SDL_MapRGBA(surface_pixels_->format, 0xff, 0xff, 0x00, 0xff);

	const Uint32* pixels = GetPixels32();

	int tile_x = 0;
	int tile_y = 0;

	for (int i = 0; i < GetPixelCount(); ++i)
	{
		if (pixels[i] == empty_color)
		{
			board_[i].type_ = TileType::EMPTY;
		}
		else if (pixels[i] == ghost_gate_color)
		{
			board_[i].type_ = TileType::GHOST_GATE;
		}
		else if (pixels[i] == ghost_home_color)
		{
			board_[i].type_ = TileType::GHOST_HOME;
		}
		else if (pixels[i] == wall_color)
		{
			board_[i].type_ = TileType::WALL;
		}
		else if (pixels[i] == energizer_color)
		{
			board_[i].type_ = TileType::PATH;
			board_[i].energizer_ = true;
			board_[i].energizer_spawned_ = true;
			++energizer_count_;
		}
		else if (pixels[i] == pellet_color)
		{
			board_[i].type_ = TileType::PATH;
			board_[i].pellet_ = true;
			board_[i].pellet_spawned_ = true;
			++pellet_count_;
		}
		else if (pixels[i] == empty_path_color)
		{
			board_[i].type_ = TileType::PATH;
		}
		else if (pixels[i] == ghost_crossroad_pellet_color)
		{
			board_[i].type_ = TileType::GHOST_CROSSROAD;
			board_[i].pellet_ = true;
			board_[i].pellet_spawned_ = true;
			++pellet_count_;
		}
		else if (pixels[i] == ghost_crossroad_empty_color)
		{
			board_[i].type_ = TileType::GHOST_CROSSROAD;
		}

		board_[i].tile_size_ = tile_size_;
		board_[i].rect_.x = tile_x;
		board_[i].rect_.y = tile_y;
		board_[i].rect_.w = tile_size_;
		board_[i].rect_.h = board_[i].rect_.w;

		tile_x += tile_size_;

		if ((i + 1) % GetPixelWidth() == 0)
		{
			tile_x = 0;
			tile_y += tile_size_;
		}
	}
}

void Level::Free()
{
	if (surface_pixels_ != nullptr)
	{
		SDL_FreeSurface(surface_pixels_);
		surface_pixels_ = nullptr;
	}
}

void Level::Reset()
{
	for (Tile& tile : board_)
	{
		if (tile.pellet_)
		{
			tile.pellet_spawned_ = true;
		}
		else if (tile.energizer_)
		{
			tile.energizer_spawned_ = true;
		}
	}
}

int Level::TileDistance(const Tile& source, const Tile& target)
{
	const int x_distance = std::abs(target.rect_.x - source.rect_.x) / tile_size_;
	const int y_distance = std::abs(target.rect_.y - source.rect_.y) / tile_size_;

	return x_distance + y_distance;
}

Tile* Level::GetTile(int x, int y)
{
	if (x < 0 || y < 0 || x >= constants::board_width || y >= constants::board_height)
	{
		return nullptr;
	}

	const int index = (y / tile_size_) * GetPixelWidth() + (x / tile_size_);
	
	if (index < 0 || index >= static_cast<int>(board_.size()))
	{
		return nullptr;
	}
	
	return &board_[index];
}

Tile* Level::GetUpperTile(int x, int y)
{
	if (y < tile_size_ && y >= 0)
	{
		y += constants::board_height;
	}

	return GetTile(x, y - tile_size_);
}
	
Tile* Level::GetLowerTile(int x, int y)
{
	if (y >= constants::board_height - tile_size_ && y < constants::board_height)
	{
		y -= constants::board_height;
	}

	return GetTile(x, y + tile_size_);
}

Tile* Level::GetLeftTile(int x, int y)
{
	if (x < tile_size_ && x >= 0)
	{
		x += constants::board_width;
	}

	return GetTile(x - tile_size_, y);
}

Tile* Level::GetRightTile(int x, int y)
{
	if (x >= constants::board_width - tile_size_ && x < constants::board_width)
	{
		x -= constants::board_width;
	}

	return GetTile(x + tile_size_, y);
}

std::vector<Tile*> Level::GetNeighborTiles(int x, int y)
{
	return { GetLeftTile(x, y), GetRightTile(x, y), GetUpperTile(x, y), GetLowerTile(x, y) };
}

int Level::GetPixelWidth()
{
	return pixel_width_;
}
	
int Level::GetPixelHeight()
{
	return pixel_height_;
}

int Level::GetPixelCount()
{
	return pixel_count_;
}

SDL_Surface* Level::GetPixelSurface()
{
	return surface_pixels_;
}

Uint32* Level::GetPixels32()
{
	pixels_ = nullptr;

	if (surface_pixels_ != nullptr)
	{
		pixels_ = static_cast<Uint32*>(surface_pixels_->pixels);
	}

	return pixels_;
}

Uint32 Level::GetPitch32()
{
	Uint32 pitch = 0;

	if (surface_pixels_ != nullptr)
	{
		pitch = surface_pixels_->pitch / 4;
	}

	return pitch;
}