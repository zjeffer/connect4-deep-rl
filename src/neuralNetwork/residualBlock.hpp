#pragma once

#include "../common.hpp"

/**
 * @brief A residual block consists of multiple convolutional layers with skip connections.
 *
 */
struct ResidualBlockImpl : public torch::nn::Module
{
    /**
     * @brief Construct a new residual block
     *
     * @param inputs: the amount of input filters
     * @param filters: the amount of hidden filters
     * @param outputs: the amount of output filters
     */
    ResidualBlockImpl(int inputs, int filters, int outputs)
    {
        register_module("conv1", conv1 = torch::nn::Conv2d(torch::nn::Conv2dOptions(inputs, filters, 3).padding(1).stride(1)));
        register_module("batchNorm1", batchNorm1 = torch::nn::BatchNorm2d(filters));

        register_module("conv2", conv2 = torch::nn::Conv2d(torch::nn::Conv2dOptions(filters, outputs, 3).padding(1).stride(1)));
        register_module("batchNorm2", batchNorm2 = torch::nn::BatchNorm2d(outputs));
    }

    /**
     * @brief Method to make a forward pass through the neural network
     *
     * @param input: the input tensor
     * @return torch::Tensor: the output tensor
     */
    torch::Tensor forward(torch::Tensor const& input)
    {
        torch::Tensor x = input;
        // first conv block
        x = batchNorm1(conv1(input));
        x = torch::relu(x);
        // second conv block
        x = batchNorm2(conv2(x));
        // skip connection, then relu
        x = torch::relu(x + input);
        return x;
    }

    torch::nn::Conv2d      conv1 = nullptr, conv2 = nullptr;
    torch::nn::BatchNorm2d batchNorm1 = nullptr, batchNorm2 = nullptr;
};
TORCH_MODULE(ResidualBlock);