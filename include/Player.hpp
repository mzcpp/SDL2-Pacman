#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"

#include <SDL2/SDL.h>

class Player : public Entity
{
private:
	Direction queued_direction_;

public:
	Player(Game* game);

	~Player() override;

	void HandleEvent(SDL_Event* e);
	
	void Tick() override;
	
	void Render() override;

	void Spawn();

	bool Move(Direction direction);
	
	void EatPellet();

	void EatEnergizer();

	Tile* GetNextTileInDirection(Direction direction);

	void SetDirection(Direction next_direction);
};

#endif