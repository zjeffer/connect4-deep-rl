#pragma once

#include <torch/torch.h>

#include "../common.hpp"
#include "cell.hpp"

class Environment
{
  public:
    /**
     * @brief Create a connect 4 environment
     *
     * @param rows: the height of the board
     * @param cols: the width of the board
     */
    Environment(int rows = 6, int cols = 7);

    /**
     * @brief Create a connect 4 environment based on a given board and the
     * current player
     *
     * @param board: the board with pieces
     * @param currentPlayer: describes whose turn it is
     */
    Environment(torch::Tensor board, ePlayer currentPlayer);

    /**
     * @brief Environment's copy constructor
     *
     * @param other: the object to copy
     */
    Environment(std::shared_ptr<Environment> const& other);

    /**
     * @brief Destroy the Environment object
     *
     */
    ~Environment();

    /**
     * @brief Create a new board filled with zeros. Reset everything.
     *
     * @param rows: the height of the new board
     * @param cols: the width of the new board
     */
    void newEnvironment(int rows, int cols);

    /**
     * @brief Get the current player
     *
     * @return ePlayer
     */
    [[nodiscard]] ePlayer getCurrentPlayer() const;

    /**
     * @brief Set the current player
     *
     * @param player
     */
    void setCurrentPlayer(ePlayer player);

    /**
     * @brief Switch the player
     *
     */
    void togglePlayer();

    /**
     * @brief Make a move by putting a piece in the given column.
     * Exits on error.
     *
     * @param column
     */
    void makeMove(int column);

    /**
     * @brief Undo the most recent move.
     *
     * @return true if successful, else false
     */
    bool undoMove();

    /**
     * @brief Return true if the given move is a legal move
     *
     * @param column: the column to put the piece in
     * @return bool
     */
    [[nodiscard]] bool isValidMove(int column) const;

    /**
     * @brief Get the amount of rows
     *
     * @return int
     */
    [[nodiscard]] int getRows() const;

    /**
     * @brief Get the amount of columns
     *
     * @return int
     */
    [[nodiscard]] int getCols() const;

    /**
     * @brief Get the player of a specific cell
     *
     * @param row: the cell's row
     * @param column: the cell's column
     * @return ePlayer
     */
    [[nodiscard]] ePlayer getPlayerAtPiece(int row, int column) const;

    /**
     * @brief Get the board
     *
     * @return torch::Tensor
     */
    [[nodiscard]] torch::Tensor getBoard();

    /**
     * @brief Set the board
     *
     * @param board
     */
    void setBoard(torch::Tensor board);

    /**
     * @brief Get the vector of possible moves in the current position.
     *
     * @return std::vector<int>: a vector of columns where the player can put
     * pieces.
     */
    [[nodiscard]] std::vector<int> getValidMoves();

    /**
     * @brief Algorithm to check if the last move was a winning move.
     *
     * @return true if the player won.
     */
    [[nodiscard]] bool currentPlayerHasConnected4() const;

    /**
     * @brief Return true if there are columns that are not full.
     *
     * @return bool
     */
    [[nodiscard]] bool hasValidMoves() const;

    /**
     * @brief If there is a winner, return it.
     *
     * @return ePlayer: returns ePlayer::None if no winner
     */
    [[nodiscard]] ePlayer getWinner() const;

    /**
     * @brief Print the current board
     *
     */
    void print();

    /**
     * @brief Print all previous moves in the history
     *
     */
    void printHistory();

  private:
    torch::Tensor m_Board;
    int           m_Rows;
    int           m_Cols;
    ePlayer       m_CurrentPlayer = ePlayer::YELLOW;

    std::vector<Cell> m_BoardHistory;
};