#include "cell.hpp"

Cell::Cell(int row, int column, ePlayer player)
  : m_Row(row)
  , m_Col(column)
  , m_Player(player)
{
}

Cell::~Cell() {}

int const& Cell::getRow() const
{
    return m_Row;
}

int const& Cell::getCol() const
{
    return m_Col;
}

ePlayer Cell::getPlayer() const
{
    return m_Player;
}

void Cell::setPlayer(ePlayer player)
{
    m_Player = player;
}
