#pragma once

#include "utils/selfPlaySettings.hpp"
#include "connect4/environment.hpp"
#include "agent.hpp"
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

	std::shared_ptr<Environment> getEnvironment() const;

	

  private:
	SelfPlaySettings* m_Settings = nullptr;
	std::shared_ptr<Environment> m_Env = nullptr;
	std::string m_GameID = "";

	std::vector<Agent*> m_Agents;

	std::vector<MemoryElement> m_Memory;

};