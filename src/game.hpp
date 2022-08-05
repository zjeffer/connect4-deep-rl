#pragma once

#include "settings.hpp"
#include "connect4/environment.hpp"
#include "common.hpp"

class Game {
  public:
	Game(Settings* settings);
	~Game();

	ePlayer playGame();

	bool playMove();


  private:
	Settings* m_Settings = nullptr;
	Environment* m_Env = nullptr;
};