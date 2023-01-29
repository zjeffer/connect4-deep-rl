#pragma once

#include "../common.hpp"
#include "../connect4/environment.hpp"
#include "../game.hpp"
#include "settings.hpp"

namespace Test
{

void testHorizontalWin();

void testVerticalWin();

void testDiagonalWin();

void testEasyPuzzle();

void testStochasticDistribution();

void testReadAndWriteMemoryElement();

void runTests();

} // namespace Test