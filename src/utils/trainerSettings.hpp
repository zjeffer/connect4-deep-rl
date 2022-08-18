#pragma once

#include "settings.hpp"


class TrainerSettings : public Settings {
public:
	TrainerSettings();
	~TrainerSettings();

	void setLearningRate(float learningRate);
	float getLearningRate();

	void setBatchSize(int batchSize);
	int getBatchSize();

private:
	float m_LearningRate = 0.02f;
	int m_BatchSize = 64;
};