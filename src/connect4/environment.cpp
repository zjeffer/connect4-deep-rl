#include "environment.hpp"

Environment::Environment(int rows, int cols)
  : m_Rows(rows)
  , m_Cols(cols)
{
    newEnvironment(rows, cols);
}

Environment::Environment(torch::Tensor board, ePlayer currentPlayer)
  : m_Board(board.detach().clone())
  , m_Rows(m_Board.size(0))
  , m_Cols(m_Board.size(1))
  , m_CurrentPlayer(currentPlayer)
{
}

Environment::Environment(std::shared_ptr<Environment> const& other)
  : m_Board(other->m_Board.detach().clone())
  , m_Rows(other->m_Rows)
  , m_Cols(other->m_Cols)
  , m_CurrentPlayer(other->m_CurrentPlayer)
  , m_BoardHistory(other->m_BoardHistory)
{
}

Environment::~Environment()
{
    // std::cout << "Environment destructor" << std::endl;
}

void Environment::newEnvironment(int rows, int cols)
{
    if (cols < 4 || rows < 4)
    {
        LFATAL << "Invalid width and/or height. Must both be greater or "
                      "equal to 4.";
    }
    m_Board         = torch::zeros({rows, cols});
    m_Rows          = rows;
    m_Cols          = cols;
    m_CurrentPlayer = ePlayer::YELLOW;
    m_BoardHistory  = std::vector<Cell>();
}

ePlayer Environment::getCurrentPlayer() const
{
    return m_CurrentPlayer;
}

void Environment::setCurrentPlayer(ePlayer player)
{
    m_CurrentPlayer = player;
}

void Environment::togglePlayer()
{
    setCurrentPlayer(m_CurrentPlayer == ePlayer::YELLOW ? ePlayer::RED : ePlayer::YELLOW);
}

void Environment::makeMove(int column)
{
    if (column < 0 || column >= m_Cols){
        LFATAL << "Column not in range 0 <= col < m_Cols";
    }

    // go down the column until we find an non-empty cell
    int row = m_Rows - 1;
    try
    {
        while (m_Board[row][column].item<int>() != 0)
        {
            row--;
        }
    }
    catch (c10::IndexError& e)
    {
        // out of bounds
        LWARN << m_Board;
        LFATAL << "Out of bounds: " << e.what();
    }

    // fill cell with currentPlayer's piece
    if (m_Board[row][column].item<int>() != 0)
    {
        LFATAL << "Error: cell is already filled";
    }
    m_Board[row][column] = static_cast<int>(m_CurrentPlayer);
    m_BoardHistory.push_back(Cell{row, column, m_CurrentPlayer});

    // switch current player
    togglePlayer();
}

bool Environment::undoMove()
{
    if (m_BoardHistory.empty())
    {
        return false;
    }
    Cell cell = m_BoardHistory.back();
    m_BoardHistory.pop_back();
    m_Board[cell.getRow()][cell.getCol()] = 0;

    // switch player
    togglePlayer();
    return true;
}

bool Environment::isValidMove(int column) const
{
    // if not in board range
    if (column < 0 || column >= m_Cols)
    {
        return false;
    }
    // if top cell of the column is full
    int value = m_Board[0][column].item<int>();
    if (value == 0)
    {
        return true;
    }
    else if (value == 1 || value == 2)
    {
        return false;
    }
    throw std::runtime_error("Error: isValidMove(): value is not 0, 1 or 2, but: " + std::to_string(value));
}

int Environment::getRows() const
{
    return m_Rows;
}

int Environment::getCols() const
{
    return m_Cols;
}

ePlayer Environment::getPlayerAtPiece(int row, int column) const
{
    return static_cast<ePlayer>(m_Board[row][column].item<int>());
}

torch::Tensor & Environment::getBoard()
{
    return m_Board;
}

void Environment::setBoard(const torch::Tensor & board)
{
    m_Board = board.detach().clone();
    // set player according to amount of pieces
    torch::Tensor counts = torch::bincount(m_Board.flatten());
    if (counts[1].item<int>() > counts[2].item<int>())
    {
        setCurrentPlayer(ePlayer::RED);
    }
    else
    {
        setCurrentPlayer(ePlayer::YELLOW);
    }
}

std::vector<int> Environment::getValidMoves()
{
    std::vector<int> validMoves;
    for (int i = 0; i < m_Cols; i++)
    {
        if (isValidMove(i))
        {
            validMoves.push_back(i);
        }
    }
    return validMoves;
}

bool Environment::currentPlayerHasConnected4() const
{
    // TODO: rewrite this function?
    if (m_BoardHistory.size() < 7)
    {
        return false;
    }

    // get last played piece
    Cell      cell   = m_BoardHistory.back();
    int const row    = cell.getRow();
    int const col    = cell.getCol();
    int const player = static_cast<int>(cell.getPlayer());

    // get horizontal win
    int amountLeftOfCell  = 0;
    int amountRightOfCell = 0;
    while (col - amountLeftOfCell - 1 >= 0 && m_Board[row][col - amountLeftOfCell - 1].item<int>() == player)
    {
        amountLeftOfCell++;
    }
    while (col + amountRightOfCell + 1 < m_Cols && m_Board[row][col + amountRightOfCell + 1].item<int>() == player)
    {
        amountRightOfCell++;
    }
    if (amountLeftOfCell + amountRightOfCell >= 3)
    {
        return true;
    }

    // get vertical win
    int amountAboveCell = 0;
    int amountBelowCell = 0;
    while (row - amountAboveCell - 1 >= 0 && m_Board[row - amountAboveCell - 1][col].item<int>() == player)
    {
        amountAboveCell++;
    }
    while (row + amountBelowCell + 1 < m_Rows && m_Board[row + amountBelowCell + 1][col].item<int>() == player)
    {
        amountBelowCell++;
    }
    if (amountAboveCell + amountBelowCell >= 3)
    {
        return true;
    }

    // get diagonal win
    int amountDiagonalLeftUp    = 0;
    int amountDiagonalRightDown = 0;
    while (row - amountDiagonalLeftUp - 1 >= 0 && col - amountDiagonalLeftUp - 1 >= 0 &&
           m_Board[row - amountDiagonalLeftUp - 1][col - amountDiagonalLeftUp - 1].item<int>() == player)
    {
        amountDiagonalLeftUp++;
    }
    while (row + amountDiagonalRightDown + 1 < m_Rows && col + amountDiagonalRightDown + 1 < m_Cols &&
           m_Board[row + amountDiagonalRightDown + 1][col + amountDiagonalRightDown + 1].item<int>() == player)
    {
        amountDiagonalRightDown++;
    }
    if (amountDiagonalLeftUp + amountDiagonalRightDown >= 3)
    {
        return true;
    }

    int amountDiagonalLeftDown = 0;
    int amountDiagonalRightUp  = 0;
    while (row + amountDiagonalLeftDown + 1 < m_Rows && col - amountDiagonalLeftDown - 1 >= 0 &&
           m_Board[row + amountDiagonalLeftDown + 1][col - amountDiagonalLeftDown - 1].item<int>() == player)
    {
        amountDiagonalLeftDown++;
    }
    while (row - amountDiagonalRightUp - 1 >= 0 && col + amountDiagonalRightUp + 1 < m_Cols &&
           m_Board[row - amountDiagonalRightUp - 1][col + amountDiagonalRightUp + 1].item<int>() == player)
    {
        amountDiagonalRightUp++;
    }
    if (amountDiagonalLeftDown + amountDiagonalRightUp >= 3)
    {
        return true;
    }

    return false;
}

bool Environment::hasValidMoves() const
{
    for (int i = 0; i < m_Cols; i++)
    {
        if (isValidMove(i))
        {
            return true;
        }
    }
    return false;
}

ePlayer Environment::getWinner() const
{
    if (currentPlayerHasConnected4())
    {
        return m_CurrentPlayer;
    }
    else
    {
        return ePlayer::NONE;
    }
}

void Environment::print()
{
    std::stringstream ss;
    ss << "\nBoard: \n";
    try
    {
        for (int i = 0; i < m_Rows; i++)
        {
            for (int j = 0; j < m_Cols; j++)
            {
                int player = m_Board[i][j].item<int>();
                if (player == 0)
                {
                    ss << ".";
                }
                else if (player == static_cast<int>(ePlayer::RED))
                {
                    ss << "R";
                }
                else if (player == static_cast<int>(ePlayer::YELLOW))
                {
                    ss << "Y";
                }
                else
                {
                    throw std::runtime_error(
                      "Error: Environment::print(): "
                      "player is not 0, 1 or 2, but: " +
                      std::to_string(player));
                }
                ss << " ";
            }
            ss << std::endl;
        }
    }
    catch (std::runtime_error& e)
    {
        LFATAL << "Error in Environment::print(): " << e.what();
    }
    ss << "Current player: " << static_cast<int>(m_CurrentPlayer);
    LINFO << ss.str();
}

void Environment::printHistory()
{
    if (m_BoardHistory.size() == 0)
    {
        LINFO << "History: no moves played yet.";
        return;
    }
    LINFO << "History: ";
    std::stringstream ss;
    ss << "\n";
    for (int i = 0; i < (int)m_BoardHistory.size(); i++)
    {
        Cell cell = m_BoardHistory[i];
        ss << cell.getRow() << " " << cell.getCol() << " " << static_cast<int>(cell.getPlayer()) << std::endl;
    }
    LINFO << ss.str();
}