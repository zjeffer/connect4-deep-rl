#pragma once

#include "player.hpp"

class Cell {
  public:
	Cell(int row, int col, ePlayer player);
	~Cell();

	[[nodiscard]] int getRow() const;
	[[nodiscard]] int getCol() const;
	[[nodiscard]] ePlayer getPlayer() const;
	void setPlayer(ePlayer player);

  private:
	int m_Row;
	int m_Col;
	ePlayer m_Player;
};