#pragma once

// torch
#include <torch/torch.h>

#include "cell.hpp"

class Environment {
  public:
	Environment(int width = 7, int height = 6);
	~Environment();

	void newEnvironment(int width, int height);

	[[nodiscard]] ePlayer getCurrentPlayer() const;
	void setPlayer(ePlayer player);
	void togglePlayer();

	bool makeMove(int column);
	bool undoMove();
	[[nodiscard]] bool isValidMove(int column) const;

	[[nodiscard]] int getWidth() const;
	[[nodiscard]] int getHeight() const;
	[[nodiscard]] ePlayer getPlayerAtPiece(int row, int column) const;

	[[nodiscard]] bool isGameOver() const;

	void print();
	void printHistory();

  private:
	int m_Width;
	int m_Height;
	ePlayer m_CurrentPlayer = ePlayer::YELLOW;

	torch::Tensor m_Board;

	std::vector<Cell> m_BoardHistory;
};