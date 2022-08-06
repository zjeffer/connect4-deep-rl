#pragma once

#include <torch/torch.h>
#include <string>
#include <time.h>

#include "types.hpp"

namespace utils {
	std::string getTimeString();

	torch::Tensor boardToTensor(Connect4Board board);

	Connect4Board tensorToBoard(torch::Tensor tensor);
}