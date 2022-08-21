#pragma once

#include "player.hpp"

class Cell {
  public:
	Cell(int row, int col, ePlayer player);
	~Cell();

	[[nodiscard]] const int& getRow() const;
	[[nodiscard]] const int& getCol() const;
	[[nodiscard]] ePlayer getPlayer() const;
	void setPlayer(ePlayer player);

  private:
	const int m_Row;
	const int m_Col;
	ePlayer m_Player;
};