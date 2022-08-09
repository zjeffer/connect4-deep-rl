#include "trainerSettings.hpp"

TrainerSettings::TrainerSettings () {

}

TrainerSettings::~TrainerSettings () {

}

void TrainerSettings::setLearningRate (float learningRate) {
	m_LearningRate = learningRate;
}

float TrainerSettings::getLearningRate () {
	return m_LearningRate;
}

void TrainerSettings::setBatchSize (int batchSize) {
	m_BatchSize = batchSize;
}

int TrainerSettings::getBatchSize () {
	return m_BatchSize;
}
