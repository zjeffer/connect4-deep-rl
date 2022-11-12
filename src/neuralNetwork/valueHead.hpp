#pragma once

#include "../common.hpp"

/**
 * @brief The value output architecture of the AlphaZero network
 *
 */
struct ValueHeadImpl : public torch::nn::Module
{
    /**
     * @brief Construct a new value head
     *
     * @param inputFilters: The amount of input filters. Must be equal to the previous layer's output filters.
     * @param valueFilters: The amount of filters in the hidden layers.
     * @param width: The width of each plane.
     * @param height: The height of each plane
     * @param linearNeurons: the amount of neurons in the output layer
     */
    ValueHeadImpl(int inputFilters, int valueFilters, int width, int height, int linearNeurons)
    {
        int const valueArraySize = width * height * valueFilters;

        convValue      = register_module("convValue", torch::nn::Conv2d(torch::nn::Conv2dOptions(inputFilters, valueFilters, 1).stride(1)));
        batchNormValue = register_module("batchNormValue", torch::nn::BatchNorm2d(valueFilters));

        linearValue1 = register_module("linearValue1", torch::nn::Linear(valueArraySize, linearNeurons));
        linearValue2 = register_module("linearValue2", torch::nn::Linear(linearNeurons, 1));
    }

    /**
     * @brief Method to make a forward pass through the neural network
     *
     * @param input: the input tensor
     * @return torch::Tensor: the output tensor
     */
    torch::Tensor forward(torch::Tensor const& input)
    {
        int64_t size = input.size(0);

        auto value = convValue(input);
        value      = value.view({size, -1});
        value      = linearValue1(value);
        value      = torch::relu(value);

        value = linearValue2(value);
        value = torch::tanh(value);

        return value;
    }

  private:
    torch::nn::Conv2d      convValue      = nullptr;
    torch::nn::BatchNorm2d batchNormValue = nullptr;
    torch::nn::Linear      linearValue1   = nullptr;
    torch::nn::Linear      linearValue2   = nullptr;
};
TORCH_MODULE(ValueHead);
