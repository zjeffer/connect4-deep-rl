#include "connect4/environment.hpp"
#include "common.hpp"


void testHorizontalWin(){
	LOG(INFO) << "Testing horizontal win";
	Environment env(6, 7);
	env.makeMove(0);
	env.makeMove(0);
	env.makeMove(1);
	env.makeMove(1);
	env.makeMove(2);
	env.makeMove(2);
	env.makeMove(3);
	env.print();

	assert(env.currentPlayerHasConnected4());
}

void testVerticalWin(){
	LOG(INFO) << "Testing vertical win";
	Environment env(6, 7);
	env.makeMove(0);
	env.makeMove(1);
	env.makeMove(0);
	env.makeMove(1);
	env.makeMove(0);
	env.makeMove(1);
	env.makeMove(0);
	env.print();
	assert(env.currentPlayerHasConnected4());
}

void testDiagonalWin(){
	LOG(INFO) << "Testing diagonal win (from bottom left to top right)";
	Environment env(6, 7);
	env.makeMove(0);
	env.makeMove(1);
	env.makeMove(1);
	env.makeMove(2);
	env.makeMove(2);
	env.makeMove(3);
	env.makeMove(2);
	env.makeMove(3);
	env.makeMove(3);
	env.makeMove(1);
	env.makeMove(3);
	env.print();
	assert(env.currentPlayerHasConnected4());

	LOG(INFO) << "Testing diagonal win (from top left to bottom right)";
	env.newEnvironment(6, 7);
	env.makeMove(0);
	env.makeMove(0);
	env.makeMove(0);
	env.makeMove(1);
	env.makeMove(0);
	env.makeMove(1);
	env.makeMove(3);
	env.makeMove(2);
	env.makeMove(2);
	env.makeMove(2);
	env.makeMove(1);
	assert(env.currentPlayerHasConnected4());
}