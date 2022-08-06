#pragma once
#undef LOG
#include "logger/logger.hpp"
#include <random>

inline bool g_running = true;

inline std::shared_ptr<Logger> logger;

inline std::uniform_int_distribution<int> g_uniform_int_dist;
inline std::default_random_engine g_generator;
inline std::gamma_distribution<double> g_gamma_distribution(0.3, 1.0);
