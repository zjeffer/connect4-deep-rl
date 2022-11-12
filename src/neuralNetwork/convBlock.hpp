#pragma once

#include "../common.hpp"

/**
 * @brief A set of layers reprenting a convolutional block
 *
 */
struct ConvBlockImpl : public torch::nn::Module
{
    /**
     * @brief Construct a new convolutional block
     *
     * @param input_filters: The amount of input filters
     * @param output_filters: The amount of output filters
     */
    ConvBlockImpl(int input_filters, int output_filters)
    {
        conv1 = register_module("conv1", torch::nn::Conv2d(torch::nn::Conv2dOptions(input_filters, output_filters, 3).padding(1).stride(1)));

        batchNorm1 = register_module("batchNorm1", torch::nn::BatchNorm2d(output_filters));
    }

    /**
     * @brief Method to make a forward pass through the neural network
     *
     * @param x: the input tensor
     * @return torch::Tensor: the output tensor
     */
    torch::Tensor forward(torch::Tensor const& x)
    {
        // convolutional layer, then batch normalisation, then ReLU
        return torch::relu(batchNorm1(conv1(x)));
    }

    torch::nn::Conv2d      conv1      = nullptr;
    torch::nn::BatchNorm2d batchNorm1 = nullptr;
};
TORCH_MODULE(ConvBlock);
