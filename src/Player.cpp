#include "Player.hpp"
#include "Game.hpp"
#include "Constants.hpp"
#include "Tile.hpp"

#include <SDL2/SDL.h>

#include <iostream>

Player::Player(Game* game) : Entity(game), queued_direction_(Direction::NONE)
{
}

Player::~Player()
{
}

void Player::HandleEvent(SDL_Event* e)
{
	if (e->type == SDL_KEYDOWN)
	{
		if (e->key.keysym.sym == SDLK_UP)
		{
			SetDirection(Direction::UP);
		}
		if (e->key.keysym.sym == SDLK_DOWN)
		{
			SetDirection(Direction::DOWN);
		}
		if (e->key.keysym.sym == SDLK_LEFT)
		{
			SetDirection(Direction::LEFT);
		}
		if (e->key.keysym.sym == SDLK_RIGHT)
		{
			SetDirection(Direction::RIGHT);
		}
	}
}

void Player::Tick()
{
	if (!Move(queued_direction_))
	{
		Move(direction_);
	}

	if (current_tile_->pellet_spawned_)
	{
		EatPellet();
	}
	else if (current_tile_->energizer_spawned_)
	{
		EatEnergizer();
	}
}

void Player::Render()
{
	//DebugNeighbors();

	SDL_SetRenderDrawColor(game_->renderer_, 0xff, 0xff, 0x00, 0xff);
	SDL_RenderFillRect(game_->renderer_, &current_tile_->rect_);
}

void Player::Spawn()
{
	current_tile_ = level_->GetTile(416, 736);
	direction_ = Direction::LEFT;
	queued_direction_ = Direction::NONE;
}

bool Player::Move(Direction direction)
{
	Tile* next_tile = GetNextTileInDirection(direction);

	if (next_tile == nullptr || next_tile->type_ == TileType::GHOST_GATE)
	{
		return false;
	}

	if (next_tile != nullptr && !next_tile->IsWall())
	{
		current_tile_ = next_tile;

		if (queued_direction_ == direction)
		{
			direction_ = queued_direction_;
			queued_direction_ = Direction::NONE;
		}

		return true;
	}

	return false;
}

void Player::EatPellet()
{
	current_tile_->pellet_spawned_ = false;
	--level_->pellet_count_;
	game_->score_ += 5;
	game_->UpdateScoreTexture();
}

void Player::EatEnergizer()
{
	current_tile_->energizer_spawned_ = false;
	--level_->energizer_count_;
	game_->score_ += 50;
	game_->UpdateScoreTexture();
}

Tile* Player::GetNextTileInDirection(Direction direction)
{
	if (direction == Direction::LEFT)
	{	
		return level_->GetLeftTile(current_tile_->rect_.x, current_tile_->rect_.y);
	}
	else if (direction == Direction::RIGHT)
	{	
		return level_->GetRightTile(current_tile_->rect_.x, current_tile_->rect_.y);
	}
	else if (direction == Direction::UP)
	{
		return level_->GetUpperTile(current_tile_->rect_.x, current_tile_->rect_.y);
	}
	else if (direction == Direction::DOWN)
	{
		return level_->GetLowerTile(current_tile_->rect_.x, current_tile_->rect_.y);
	}

	return nullptr;
}

void Player::SetDirection(Direction next_direction)
{
	Tile* next_tile = GetNextTileInDirection(next_direction);

	if (next_tile == nullptr || next_tile->type_ == TileType::GHOST_GATE)
	{
		return;
	}

	if (!next_tile->IsWall())
	{
		direction_ = next_direction;
	}
	else if (direction_ != next_direction)
	{
		queued_direction_ = next_direction;
	}
}
