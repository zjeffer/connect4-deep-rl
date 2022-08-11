#pragma once

#include <torch/torch.h>
#include <string>
#include <time.h>

#include "types.hpp"

namespace utils {
	std::string getTimeString();

	std::vector<int> boardToVector(torch::Tensor tensor);
}