#ifndef GHOST_HPP
#define GHOST_HPP

#include "Entity.hpp"

#include <SDL2/SDL.h>

enum class GhostType
{
	BLINKY, INKY, PINKY, CLYDE
};

enum class GhostMode
{
	SCATTER, CHASE, FRIGHTENED, RESPAWNING
};

class Ghost : public Entity
{
public:
	GhostType type_;
	GhostMode mode_;
private:
	Tile* target_tile_;
	Tile* scatter_target_tile_;
	Tile* home_porch_target_tile_;
	Tile* home_target_tile_;

public:
	Ghost(Game* game, Level* level, GhostType type);

	~Ghost() override;

	void Spawn();

	void Tick() override;
	
	void Render() override;

	void Move();
	
	void UpdateTargetCells();
};

#endif