#pragma once

#include "utils/selfPlaySettings.hpp"
#include "connect4/environment.hpp"
#include "agent.hpp"
#include "common.hpp"
#include "utils/types.hpp"


class Game {
  public:
	Game(SelfPlaySettings* selfPlaySettings);
	~Game();

	ePlayer playGame();

	bool playMove();

	void addElementToMemory(MemoryElement element);

	void updateMemoryWithWinner(ePlayer winner);

	bool saveMemoryToFile();




  private:
	SelfPlaySettings* m_Settings = nullptr;
	Environment* m_Env = nullptr;
	std::string m_GameID = "";

	std::vector<Agent*> m_Agents;

	std::vector<MemoryElement> memory;

};