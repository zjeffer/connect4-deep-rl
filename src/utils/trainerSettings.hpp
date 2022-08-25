#pragma once

#include "settings.hpp"


class TrainerSettings : public Settings {
public:
	/**
	 * @brief Construct a new settings object
	 * 
	 */
	TrainerSettings();

	/**
	 * @brief Set the trainer's learning rate
	 * 
	 * @param learningRate 
	 */
	void setLearningRate(float learningRate);

	/**
	 * @brief Get the learning rate
	 * 
	 * @return float 
	 */
	[[nodiscard]] float getLearningRate() const;

	/**
	 * @brief Set the trainer's batch size
	 * 
	 * @param batchSize 
	 */
	void setBatchSize(int batchSize);
	[[nodiscard]] int getBatchSize() const;

private:
	float m_LearningRate = 0.02f;
	int m_BatchSize = 64;
};