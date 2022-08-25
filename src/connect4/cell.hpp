#pragma once

#include "player.hpp"

/**
 * @brief
 *
 */
class Cell {
  public:
	/**
	 * @brief Construct a cell object
	 *
	 * @param row: The horizontal location
	 * @param col: The vertical location
	 * @param player: which player has a piece in this cell (ePlayer::None if
	 * empty)
	 */
	Cell(int row, int col, ePlayer player);

	/**
	 * @brief Destroy the Cell object
	 *
	 */
	~Cell();

	/**
	 * @brief Get the cell's row
	 *
	 * @return const int&
	 */
	[[nodiscard]] const int &getRow() const;

	/**
	 * @brief Get the cell's column
	 *
	 * @return const int&
	 */
	[[nodiscard]] const int &getCol() const;

	/**
	 * @brief Get the Player
	 *
	 * @return ePlayer
	 */
	[[nodiscard]] ePlayer getPlayer() const;

	/**
	 * @brief Set the Player
	 *
	 * @param player
	 */
	void setPlayer(ePlayer player);

  private:
	const int m_Row;
	const int m_Col;
	ePlayer m_Player;
};