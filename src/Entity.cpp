#include "Entity.hpp"
#include "Game.hpp"
#include "Level.hpp"

#include <vector>

Entity::Entity(Game* game) : game_(game), level_(nullptr), direction_(Direction::LEFT), current_tile_(nullptr)
{
}

Entity::~Entity()
{
}

void Entity::SetLevel(Level* level)
{
	level_ = level;
}

Tile* Entity::GetCurrentTile()
{
	return current_tile_;
}

void Entity::DebugNeighbors()
{
	const std::vector<Tile*> neighbors = level_->GetNeighborTiles(current_tile_->rect_.x, current_tile_->rect_.y);

	for (std::size_t i = 0; i < neighbors.size(); ++i)
	{
		if (neighbors[i] == nullptr)
		{
			continue;
		}

		if (neighbors[i]->IsWall())
		{
			SDL_SetRenderDrawColor(game_->renderer_, 0xff, 0x00, 0x00, 0xff);
		}
		else
		{
			SDL_SetRenderDrawColor(game_->renderer_, 0x00, 0xff, 0x00, 0xff);
		}

		SDL_RenderFillRect(game_->renderer_, &neighbors[i]->rect_);
	}
}

Direction Entity::GetDirection()
{
	return direction_;
}
