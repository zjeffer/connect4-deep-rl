#pragma once

#include "../common.hpp"

/**
 * @brief The policy output architecture of the AlphaZero network
 *
 */
struct PolicyHeadImpl : public torch::nn::Module
{
    /**
     * @brief Construct a new policy head
     *
     * @param inputFilters The amount of input filters. Must be equal to the previous layer's output filters.
     * @param policyFilters The amount of filters in the hidden layers.
     * @param width The width of each plane.
     * @param height The height of each plane.
     * @param outputs The amount of outputs in the output layer
     */
    PolicyHeadImpl(int inputFilters, int policyFilters, int width, int height, int outputs)
    {
        int policyArraySize = width * height * policyFilters;

        convPolicy      = register_module("convPolicy", torch::nn::Conv2d(torch::nn::Conv2dOptions(inputFilters, policyFilters, 1).stride(1)));
        batchNormPolicy = register_module("batchNormPolicy", torch::nn::BatchNorm2d(policyFilters));
        linearPolicy    = register_module("linearPolicy", torch::nn::Linear(policyArraySize, outputs));
    }

    /**
     * @brief Method to make a forward pass through the neural network
     *
     * @param input: the input tensor
     * @return torch::Tensor: the output tensor
     */
    torch::Tensor forward(torch::Tensor const& input)
    {
        int64_t batch_size = input.size(0);

        // conv block
        auto pol = convPolicy(input);
        pol      = batchNormPolicy(pol);
        pol      = torch::relu(pol);

        // flatten
        pol = pol.view({batch_size, -1});

        // linear
        pol = linearPolicy(pol);

        // softmax activation
        pol = torch::softmax(pol, 1);
        return pol;
    }

  private:
    torch::nn::Conv2d      convPolicy      = nullptr;
    torch::nn::BatchNorm2d batchNormPolicy = nullptr;
    torch::nn::Linear      linearPolicy    = nullptr;
};
TORCH_MODULE(PolicyHead);
