#include "trainerSettings.hpp"

TrainerSettings::TrainerSettings() {}

void TrainerSettings::setLearningRate(float learningRate) { m_LearningRate = learningRate; }

float TrainerSettings::getLearningRate() const { return m_LearningRate; }

void TrainerSettings::setBatchSize(int batchSize) { m_BatchSize = batchSize; }

int TrainerSettings::getBatchSize() const { return m_BatchSize; }
