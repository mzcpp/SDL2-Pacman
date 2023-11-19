#ifndef GAME_HPP
#define GAME_HPP

#include "Texture.hpp"
#include "Tile.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Ghost.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <memory>
#include <vector>
#include <array>

class Game
{
private:
	bool running_;
	bool initialized_;

public:
	bool game_over_;
	bool level_completed_;

	int score_;
	int lives_;
	int levels_cleared_;
	int game_ticks_;
	double mode_timer_;

private:
	std::unique_ptr<Level> level_;
	std::unique_ptr<Player> player_;
	std::vector<std::unique_ptr<Ghost>> ghosts_;

	std::unique_ptr<Texture> game_over_texture_;
	std::unique_ptr<Texture> level_completed_texture_;
	std::unique_ptr<Texture> score_texture_;
	std::unique_ptr<Texture> lives_texture_;
	std::unique_ptr<Texture> levels_cleared_texture_;

	SDL_Rect board_viewport_;
	SDL_Rect info_viewport_;

public:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	TTF_Font* font_;

	Game();

	~Game();

	bool Initialize();

	void Finalize();

	void HandleEvents();
	
	void Tick();
	
	void Render();

	void RenderBoard();

	void RenderInfo();

	void Run();

	void Stop();

	void Reset(bool reset_pellets = true);

	void UpdateScoreTexture();
	
	void UpdateLivesTexture();
	
	void UpdateLevelsClearedTexture();
	
	Player* GetPlayer();
};

#endif