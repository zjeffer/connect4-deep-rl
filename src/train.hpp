#pragma once

#include "neuralNetwork.hpp"
#include "utils/trainerSettings.hpp"


class Trainer {
public:
  Trainer(TrainerSettings* settings);
  ~Trainer();

  void train();

private:
	std::shared_ptr<NeuralNetwork> m_NN;
  TrainerSettings* m_Settings;
	
};