#include "environment.hpp"
#include "../common.hpp"

Environment::Environment(int width, int height) : m_Width(width), m_Height(height) {
	newEnvironment(width, height);
}

Environment::~Environment(){
	LOG(DEBUG) << "Environment destructor";
}

void Environment::newEnvironment(int width, int height){
	if (width < 4 || height < 4) {
		LOG(FATAL) << "Invalid width and/or height. Must both be greater or equal to 4.";
		exit(EXIT_FAILURE);
	}
	m_Board = torch::zeros({height, width});
	m_Width = width;
	m_Height = height;
	m_CurrentPlayer = ePlayer::YELLOW;
	m_BoardHistory = std::vector<Cell>();
}


ePlayer Environment::getCurrentPlayer() const{
	return m_CurrentPlayer;
}

void Environment::setPlayer(ePlayer player){
	m_CurrentPlayer = player;
}

void Environment::togglePlayer(){
	setPlayer(m_CurrentPlayer == ePlayer::YELLOW ? ePlayer::RED : ePlayer::YELLOW);
}

bool Environment::makeMove(int column) {
	if (!isValidMove(column)){
		return false;
	}
	LOG(INFO) << "Player " << static_cast<int>(m_CurrentPlayer) << " makes move at column " << column;
	// go down the column until we find an non-empty cell
	int row = m_Height - 1;
	try {
		while (m_Board[row][column].item<int>() != 0){
			row--;
		}
	} catch (std::out_of_range& e){
		// out of bounds
		LOG(FATAL) << "Out of bounds: " << e.what();
		exit(EXIT_FAILURE);
	}

	// fill cell with currentPlayer's piece
	if (m_Board[row][column].item<int>() != 0){
		LOG(WARNING) << "Error: cell is already filled";
		return false;
	}
	m_Board[row][column] = static_cast<int>(m_CurrentPlayer);
	m_BoardHistory.push_back(Cell {row, column, m_CurrentPlayer});

	// switch current player
	togglePlayer();
	return true;
}

bool Environment::undoMove() {
	if (m_BoardHistory.empty()){
		return false;
	}
	Cell cell = m_BoardHistory.back();
	m_BoardHistory.pop_back();
	m_Board[cell.getRow()][cell.getCol()] = 0;

	// switch player
	togglePlayer();
	return true;
}

bool Environment::isValidMove(int column) const{
	// if not in board range
	if (column < 0 || column >= m_Width){
		return false;
	}
	// if column is full
	if (m_Board[0][column].item<int>() != 0){
		return false;
	}
	// check if game is over
	if (isGameOver()){
		return false;
	}
	return true;
}

int Environment::getWidth() const{
	return m_Width;
}

int Environment::getHeight() const{
	return m_Height;
}

ePlayer Environment::getPlayerAtPiece(int row, int column) const{
	return static_cast<ePlayer>(m_Board[row][column].item<int>());
}

bool Environment::isGameOver() const{
	// TODO: rewrite this?
	if (m_BoardHistory.size() < 7){
		return false;
	}
	// get last played piece
	Cell cell = m_BoardHistory.back();
	const int row = cell.getRow();
	const int col = cell.getCol();
	const int player = static_cast<int>(cell.getPlayer());

	// get horizontal win
	int amountLeftOfCell = 0;
	int amountRightOfCell = 0;
	while (col - amountLeftOfCell - 1 >= 0 && m_Board[row][col - amountLeftOfCell - 1].item<int>() == player){
		amountLeftOfCell++;
	}
	while (col + amountRightOfCell + 1 < m_Width && m_Board[row][col + amountRightOfCell + 1].item<int>() == player){
		amountRightOfCell++;
	}
	if (amountLeftOfCell + amountRightOfCell >= 3){
		return true;
	}
	

	// get vertical win
	int amountAboveCell = 0;
	int amountBelowCell = 0;
	while (row - amountAboveCell - 1 >= 0 && m_Board[row - amountAboveCell - 1][col].item<int>() == player){
		amountAboveCell++;
	}
	while (row + amountBelowCell + 1 < m_Height && m_Board[row + amountBelowCell + 1][col].item<int>() == player){
		amountBelowCell++;
	}
	if (amountAboveCell + amountBelowCell >= 3){
		return true;
	}

	// get diagonal win
	int amountDiagonalLeftUp = 0;
	int amountDiagonalRightDown = 0;
	while (row - amountDiagonalLeftUp - 1 >= 0 && col - amountDiagonalLeftUp - 1 >= 0 && m_Board[row - amountDiagonalLeftUp - 1][col - amountDiagonalLeftUp - 1].item<int>() == player){
		amountDiagonalLeftUp++;
	}
	while (row + amountDiagonalRightDown + 1 < m_Height && col + amountDiagonalRightDown + 1 < m_Width && m_Board[row + amountDiagonalRightDown + 1][col + amountDiagonalRightDown + 1].item<int>() == player){
		amountDiagonalRightDown++;
	}
	if (amountDiagonalLeftUp + amountDiagonalRightDown >= 3){
		return true;
	}

	int amountDiagonalLeftDown = 0;
	int amountDiagonalRightUp = 0;
	while (row + amountDiagonalLeftDown + 1 < m_Height && col - amountDiagonalLeftDown - 1 >= 0 && m_Board[row + amountDiagonalLeftDown + 1][col - amountDiagonalLeftDown - 1].item<int>() == player){
		amountDiagonalLeftDown++;
	}
	while (row - amountDiagonalRightUp - 1 >= 0 && col + amountDiagonalRightUp + 1 < m_Width && m_Board[row - amountDiagonalRightUp - 1][col + amountDiagonalRightUp + 1].item<int>() == player){
		amountDiagonalRightUp++;
	}
	if (amountDiagonalLeftDown + amountDiagonalRightUp >= 3){
		return true;
	}

	return false;
}

void Environment::print(){
	LOG(INFO) << "Current player: " << (int)m_CurrentPlayer;
	LOG(INFO) << "Board: ";
	std::stringstream ss;
	ss << "\n";
	for (int i = 0; i < m_Height; i++){
		for (int j = 0; j < m_Width; j++){
			int value = m_Board[i][j].item<int>();
			if (value == 0){
				ss << ".";
			} else if (value == static_cast<int>(ePlayer::RED)) {
				ss << "R";
			} else if (value == static_cast<int>(ePlayer::YELLOW)) {
				ss << "Y";
			} else {
				std::cout << "\n" << std::endl;
				std::cout << m_Board << std::endl;
				throw std::runtime_error("Invalid value in board");
			}
			ss << " ";
		}
		ss << std::endl;
	}
	LOG(INFO) << ss.str();
}

void Environment::printHistory(){
	if (m_BoardHistory.size() == 0){
		LOG(INFO) << "History: no moves played yet.";
		return;
	}
	LOG(INFO) << "History: ";
	std::stringstream ss;
	ss << "\n";
	for (int i = 0; i < m_BoardHistory.size(); i++){
		Cell cell = m_BoardHistory[i];
		ss << cell.getRow() << " " << cell.getCol() << " " << static_cast<int>(cell.getPlayer()) << std::endl;
	}
	LOG(INFO) << ss.str();
}