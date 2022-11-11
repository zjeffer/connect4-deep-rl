#pragma once

#include <random>

#include "logger/logger.hpp"

// global variable to indicate the program is running
inline bool g_Running = true;

// the global logger
inline std::shared_ptr<Logger> g_Logger;

// uniform int distribution (for creating a random game ID)
inline std::uniform_int_distribution<int> g_UniformIntDist;
// the random engine for all random generation
inline std::default_random_engine g_Generator;
// gamma distribution to add dirichlet noise (TODO: implement)
inline std::gamma_distribution<double> g_GammaDist(0.3, 1.0);
