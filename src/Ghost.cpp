#include "Ghost.hpp"
#include "Game.hpp"

#include <SDL2/SDL.h>

#include <iostream>

Ghost::Ghost(Game* game, Level* level, GhostType type) : 
	Entity(game), 
	type_(type), 
	mode_(GhostMode::SCATTER), 
	target_tile_(nullptr), 
	scatter_target_tile_(nullptr), 
	home_porch_target_tile_(nullptr), 
	home_target_tile_(nullptr)
{
	SetLevel(level);
	Spawn();
}

Ghost::~Ghost()
{
}

void Ghost::Spawn()
{
	if (type_ == GhostType::BLINKY)
	{
		current_tile_ = level_->GetTile(352, 416);
		scatter_target_tile_ = level_->GetTile(864, 0);
	}
	else if (type_ == GhostType::INKY)
	{
		current_tile_ = level_->GetTile(352, 480);
		scatter_target_tile_ = level_->GetTile(864, 960);
	}
	else if (type_ == GhostType::PINKY)
	{
		current_tile_ = level_->GetTile(512, 416);
		scatter_target_tile_ = level_->GetTile(0, 0);
	}
	else if (type_ == GhostType::CLYDE)
	{
		current_tile_ = level_->GetTile(512, 480);
		scatter_target_tile_ = level_->GetTile(0, 960);
	}

	home_target_tile_ = current_tile_;
	home_porch_target_tile_ = level_->GetTile(416, 352);
	target_tile_ = home_porch_target_tile_;
}

void Ghost::Tick()
{
	Move();
	UpdateTargetCells();
}

void Ghost::Render()
{
	//DebugNeighbors();

	if (type_ == GhostType::BLINKY)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0xff, 0x00, 0x00, 0xff);
	}
	else if (type_ == GhostType::INKY)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0x00, 0xff, 0xff, 0xff);
	}
	else if (type_ == GhostType::PINKY)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0xff, 0xb8, 0xff, 0xff);
	}
	else if (type_ == GhostType::CLYDE)
	{
		SDL_SetRenderDrawColor(game_->renderer_, 0xff, 0xb8, 0x51, 0xff);
	}
	
	SDL_RenderFillRect(game_->renderer_, &current_tile_->rect_);
	//SDL_RenderFillRect(game_->renderer_, &target_tile_->rect_);
}

void Ghost::Move()
{
	const std::vector<Tile*> neighbors = level_->GetNeighborTiles(current_tile_->rect_.x, current_tile_->rect_.y);
	Tile* next_tile = nullptr;
	Direction next_direction = Direction::NONE;

	for (std::size_t i = 0; i < neighbors.size(); ++i)
	{
		if (neighbors[i] == nullptr || neighbors[i]->IsWall())
		{
			continue;
		}

		const int dir = static_cast<int>(direction_);
		const int current = static_cast<int>(i);

		if ((dir == 1 && current == 0) || (dir == 0 && current == 1) || (dir == 2 && current == 3) || (dir == 3 && current == 2))
		{
			continue;
		}

		if (current_tile_->type_ != TileType::GHOST_HOME && neighbors[i]->type_ == TileType::GHOST_GATE)
		{
			continue;
		}

		if (current_tile_->type_ == TileType::GHOST_CROSSROAD && i == 2)
		{
			continue;
		}

		if (next_tile == nullptr)
		{
			next_tile = neighbors[i];
			next_direction = static_cast<Direction>(i);
			continue;
		}

		if (level_->TileDistance(*neighbors[i], *target_tile_) < level_->TileDistance(*current_tile_, *target_tile_))
		{
			next_tile = neighbors[i];
			next_direction = static_cast<Direction>(i);
		}
	}

	current_tile_ = next_tile;
	direction_ = next_direction;
}

void Ghost::UpdateTargetCells()
{
	if (target_tile_ == home_porch_target_tile_)
	{
		if (current_tile_ == home_porch_target_tile_)
		{
			target_tile_ = scatter_target_tile_;
		}

		return;
	}

	if (mode_ == GhostMode::SCATTER)
	{
		target_tile_ = scatter_target_tile_;
		return;
	}

	if (type_ == GhostType::BLINKY)
	{
		target_tile_ = game_->GetPlayer()->GetCurrentTile();
	}
	else if (type_ == GhostType::INKY)
	{
		int opposite_direction = -1;
		int direction = static_cast<int>(game_->GetPlayer()->GetDirection());

		if (direction == 0)
		{
			opposite_direction = 1;
		}
		else if (direction == 1)
		{
			opposite_direction = 0;
		}
		else if (direction == 2)
		{
			opposite_direction = 3;
		}
		else if (direction == 3)
		{
			opposite_direction = 2;
		}

		target_tile_ = game_->GetPlayer()->GetNextTileInDirection(static_cast<Direction>(opposite_direction));
	}
	else if (type_ == GhostType::PINKY)
	{
		target_tile_ = game_->GetPlayer()->GetNextTileInDirection(game_->GetPlayer()->GetDirection());
	}
	else if (type_ == GhostType::CLYDE)
	{
		if (level_->TileDistance(*current_tile_, *game_->GetPlayer()->GetCurrentTile()) < 5)
		{
			target_tile_ = scatter_target_tile_;
		}
		else
		{
			target_tile_ = game_->GetPlayer()->GetCurrentTile();
		}
	}
}
