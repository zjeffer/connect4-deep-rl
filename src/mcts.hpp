#pragma once

#include "settings.hpp"
#include "tree/node.hpp"

class MCTS {
  public:
	MCTS(Settings *settings, Node *root);
	~MCTS();

	void run_simulations();

  private:
	Settings *m_Settings = nullptr;
};