#include "Game.hpp"
#include "Constants.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <cstdint>
#include <string>
#include <memory>

Game::Game() : 
	running_(false), 
	initialized_(false), 
	game_over_(false), 
	level_completed_(false), 
	score_(0), 
	lives_(5), 
	levels_cleared_(0), 
	game_ticks_(0), 
	mode_timer_(0.0), 
	level_(std::make_unique<Level>(this)), 
	player_(std::make_unique<Player>(this)), 
	game_over_texture_(std::make_unique<Texture>()), 
	level_completed_texture_(std::make_unique<Texture>()), 
	score_texture_(std::make_unique<Texture>()), 
	lives_texture_(std::make_unique<Texture>()), 
	levels_cleared_texture_(std::make_unique<Texture>()), 
	window_(nullptr), 
	renderer_(nullptr), 
	font_(nullptr)
{
	initialized_ = Initialize();

	if (!initialized_)
	{
		return;
	}

	level_->Initialize("res/levels/default.png");

	player_->SetLevel(level_.get());
	player_->Spawn();

	ghosts_.emplace_back(std::make_unique<Ghost>(this, level_.get(), GhostType::BLINKY));
	ghosts_.emplace_back(std::make_unique<Ghost>(this, level_.get(), GhostType::INKY));
	ghosts_.emplace_back(std::make_unique<Ghost>(this, level_.get(), GhostType::PINKY));
	ghosts_.emplace_back(std::make_unique<Ghost>(this, level_.get(), GhostType::CLYDE));

	std::for_each(ghosts_.begin(), ghosts_.end(), [this](const std::unique_ptr<Ghost>& ghost)
	{
		ghost->SetLevel(level_.get());
	});

	SDL_Color red_color = { 0xff, 0x00, 0x00, 0xff };
	SDL_Color green_color = { 0x00, 0xff, 0x00, 0xff };

	game_over_texture_->LoadFromText(renderer_, font_, "Game Over! Press 'r' to reset.", red_color);
	level_completed_texture_->LoadFromText(renderer_, font_, "Level Completed! Press 'c' to continue.", green_color);

	UpdateScoreTexture();
	UpdateLivesTexture();
	UpdateLevelsClearedTexture();

	board_viewport_.x = 0;
	board_viewport_.y = 0;
	board_viewport_.w = constants::board_width;
	board_viewport_.h = constants::board_height;

	info_viewport_.x = 0;
	info_viewport_.y = constants::board_height;
	info_viewport_.w = constants::info_width;
	info_viewport_.h = constants::info_height;

	mode_timer_ = SDL_GetTicks();	
}

Game::~Game()
{
	Finalize();
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
	{
		printf("%s\n", "Warning: Texture filtering is not enabled!");
	}

	window_ = SDL_CreateWindow(constants::game_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, constants::screen_width, constants::screen_height, SDL_WINDOW_SHOWN);

	if (window_ == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

	if (renderer_ == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	constexpr int img_flags = IMG_INIT_PNG;

	if (!(IMG_Init(img_flags) & img_flags))
	{
		printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not be initialized! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	
	font_ = TTF_OpenFont("res/font/font.ttf", 38);

	if (font_ == nullptr)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

void Game::Finalize()
{
	SDL_DestroyWindow(window_);
	window_ = nullptr;

	SDL_DestroyRenderer(renderer_);
	renderer_ = nullptr;

	TTF_CloseFont(font_);
	font_ = nullptr;

	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}

void Game::HandleEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			running_ = false;
			return;
		}
		else if (e.type == SDL_KEYDOWN)
		{
			if (game_over_ && e.key.keysym.sym == SDLK_r)
			{
				Reset();
			}
			else if (level_completed_ && e.key.keysym.sym == SDLK_c)
			{
				Reset();
			}
		}

		player_->HandleEvent(&e);
	}
}

void Game::Tick()
{
	if (++game_ticks_ % 20 == 0)
	{
		if (!game_over_ && !level_completed_)
		{
			player_->Tick();

			std::for_each(ghosts_.begin(), ghosts_.end(), [this](const std::unique_ptr<Ghost>& ghost)
			{
				ghost->Tick();

				if (player_->GetCurrentTile() == ghost->GetCurrentTile())
				{
					--lives_;

					if (lives_ == 0)
					{
						Stop();
					}
					else
					{
						Reset(false);
						UpdateLivesTexture();
					}
				}
			});
		}

		level_->Tick();
	}

	if (!game_over_ && !level_completed_)
	{
		if (ghosts_[0]->mode_ == GhostMode::SCATTER && (SDL_GetTicks() - mode_timer_) >= 7000.0)
		{
			mode_timer_ += 7000.0;

			std::for_each(ghosts_.begin(), ghosts_.end(), [](const std::unique_ptr<Ghost>& ghost)
			{
				ghost->mode_ = GhostMode::CHASE;
			});
		}
		else if (ghosts_[0]->mode_ == GhostMode::CHASE && (SDL_GetTicks() - mode_timer_) >= 20000.0)
		{
			mode_timer_ += 20000.0;

			std::for_each(ghosts_.begin(), ghosts_.end(), [](const std::unique_ptr<Ghost>& ghost)
			{
				ghost->mode_ = GhostMode::SCATTER;
			});
		}
	}
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(renderer_);

	RenderBoard();

	player_->Render();

	std::for_each(ghosts_.begin(), ghosts_.end(), [](const std::unique_ptr<Ghost>& ghost)
	{
		ghost->Render();
	});

	RenderInfo();

	SDL_RenderPresent(renderer_);
}

void Game::RenderBoard()
{
	SDL_RenderSetViewport(renderer_, &board_viewport_);
	
	level_->Render();

	SDL_RenderSetViewport(renderer_, NULL);	
}

void Game::RenderInfo()
{
	SDL_RenderSetViewport(renderer_, &board_viewport_);

	if (game_over_)
	{
		game_over_texture_->Render(renderer_, (constants::screen_width / 2) - (game_over_texture_->width_ / 2), (constants::board_height / 2) - (game_over_texture_->height_ / 2));
	}

	if (level_completed_)
	{
		level_completed_texture_->Render(renderer_, (constants::screen_width / 2) - (level_completed_texture_->width_ / 2), (constants::board_height / 2) - (level_completed_texture_->height_ / 2));
	}

	SDL_RenderSetViewport(renderer_, &info_viewport_);

	score_texture_->Render(renderer_, (constants::screen_width / 2) - (score_texture_->width_ / 2), (constants::info_height / 4));
	lives_texture_->Render(renderer_, (constants::screen_width / 10), (constants::info_height - lives_texture_->height_));
	levels_cleared_texture_->Render(renderer_, (constants::screen_width * 7 / 10) - (levels_cleared_texture_->width_ / 2), (constants::info_height - levels_cleared_texture_->height_));

	SDL_RenderSetViewport(renderer_, NULL);
}

void Game::Run()
{
	if (!initialized_)
	{
		return;
	}

	running_ = true;

	constexpr long double ms = 1.0 / 60.0;
	std::uint64_t last_time = SDL_GetPerformanceCounter();
	long double delta = 0.0;

	double timer = SDL_GetTicks();

	int frames = 0;
	int ticks = 0;

	while (running_)
	{
		const std::uint64_t now = SDL_GetPerformanceCounter();
		const long double elapsed = static_cast<long double>(now - last_time) / static_cast<long double>(SDL_GetPerformanceFrequency());

		last_time = now;
		delta += elapsed;

		HandleEvents();

		while (delta >= ms)
		{
			Tick();
			delta -= ms;
			++ticks;
		}

		//printf("%Lf\n", delta / ms);
		Render();
		++frames;

		if (SDL_GetTicks() - timer > 1000)
		{
			timer += 1000;
			//printf("Frames: %d, Ticks: %d\n", frames, ticks);
			frames = 0;
			ticks = 0;
		}
	}
}

void Game::Stop()
{
	game_over_ = true;
}

void Game::Reset(bool reset_pellets)
{
	if (game_over_)
	{
		score_ = 0;
		levels_cleared_ = 0;
		lives_ = 5;
		game_over_ = false;
		UpdateScoreTexture();
		UpdateLivesTexture();
		UpdateLevelsClearedTexture();
	}
	else if (level_completed_)
	{
		++levels_cleared_;
		level_completed_ = false;
		UpdateLevelsClearedTexture();
	}

	mode_timer_ = 0.0;

	if (reset_pellets)
	{
		level_->Reset();
	}

	player_->Spawn();

	std::for_each(ghosts_.begin(), ghosts_.end(), [](const std::unique_ptr<Ghost>& ghost)
	{
		ghost->Spawn();
	});
}

void Game::UpdateScoreTexture()
{
	SDL_Color white_color = { 0xff, 0xff, 0xff, 0xff };
	const std::string score_text = "Score: " + std::to_string(score_);
	score_texture_->LoadFromText(renderer_, font_, score_text.c_str(), white_color);
}
	
void Game::UpdateLivesTexture()
{
	SDL_Color white_color = { 0xff, 0xff, 0xff, 0xff };
	const std::string lives_text = "Lives: " + std::to_string(lives_);
	lives_texture_->LoadFromText(renderer_, font_, lives_text.c_str(), white_color);
}
	
void Game::UpdateLevelsClearedTexture()
{
	SDL_Color white_color = { 0xff, 0xff, 0xff, 0xff };
	const std::string levels_cleared_text = "Levels Cleared: " + std::to_string(levels_cleared_);
	levels_cleared_texture_->LoadFromText(renderer_, font_, levels_cleared_text.c_str(), white_color);
}

Player* Game::GetPlayer()
{
	return player_.get();
}
