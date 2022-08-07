#pragma once

#include <torch/torch.h>

#include "cell.hpp"

class Environment {
  public:
	Environment(int rows = 6, int cols = 7);
	Environment(torch::Tensor board, ePlayer currentPlayer);
	~Environment();

	void newEnvironment(int cols, int rows);

	[[nodiscard]] ePlayer getCurrentPlayer() const;
	void setPlayer(ePlayer player);
	void togglePlayer();

	void makeMove(int column);
	bool undoMove();
	[[nodiscard]] bool isValidMove(int column) const;

	[[nodiscard]] int getRows() const;
	[[nodiscard]] int getCols() const;
	[[nodiscard]] ePlayer getPlayerAtPiece(int row, int column) const;

	[[nodiscard]] torch::Tensor getBoard();
	void setBoard(torch::Tensor board);
	
	[[nodiscard]] std::vector<int> getValidMoves();

	[[nodiscard]] bool currentPlayerHasConnected4() const;
	[[nodiscard]] bool hasValidMoves() const;
	[[nodiscard]] ePlayer getWinner() const;

	void print();
	void printHistory();

  private:
	int m_Rows;
	int m_Cols;
	ePlayer m_CurrentPlayer = ePlayer::YELLOW;

	torch::Tensor m_Board;

	std::vector<Cell> m_BoardHistory;
};