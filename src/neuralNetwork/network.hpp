#pragma once

#include "../common.hpp"

#include <tuple>

#include "convBlock.hpp"
#include "policyHead.hpp"
#include "residualBlock.hpp"
#include "valueHead.hpp"

/**
 * @brief The full neural network architecture based on AlphaZero
 *
 */
struct NetworkImpl : public torch::nn::Module
{
    /**
     * @brief Construct a new Network
     *
     * @param planes the amount of input planes
     * @param width the width of each plane
     * @param height the height of each plane
     * @param outputs the amount of policy outputs
     * @param filters the amount of convolutional filters each layer
     * @param policyFilters the amount of filters in the policy layers
     * @param valueFilters the amount of filters in the value layers
     */
    NetworkImpl(int planes, int width, int height, int outputs, int filters, int policyFilters, int valueFilters)
    {
        convInput = register_module("convInput", ConvBlock(planes, filters));

        resBlock1  = register_module("resBlock1", ResidualBlock(filters, filters, filters));
        resBlock2  = register_module("resBlock2", ResidualBlock(filters, filters, filters));
        resBlock3  = register_module("resBlock3", ResidualBlock(filters, filters, filters));
        resBlock4  = register_module("resBlock4", ResidualBlock(filters, filters, filters));
        resBlock5  = register_module("resBlock5", ResidualBlock(filters, filters, filters));
        resBlock6  = register_module("resBlock6", ResidualBlock(filters, filters, filters));
        resBlock7  = register_module("resBlock7", ResidualBlock(filters, filters, filters));
        resBlock8  = register_module("resBlock8", ResidualBlock(filters, filters, filters));
        resBlock9  = register_module("resBlock9", ResidualBlock(filters, filters, filters));
        resBlock10 = register_module("resBlock10", ResidualBlock(filters, filters, filters));
        resBlock11 = register_module("resBlock11", ResidualBlock(filters, filters, filters));
        resBlock12 = register_module("resBlock12", ResidualBlock(filters, filters, filters));
        resBlock13 = register_module("resBlock13", ResidualBlock(filters, filters, filters));
        resBlock14 = register_module("resBlock14", ResidualBlock(filters, filters, filters));
        resBlock15 = register_module("resBlock15", ResidualBlock(filters, filters, filters));
        resBlock16 = register_module("resBlock16", ResidualBlock(filters, filters, filters));
        resBlock17 = register_module("resBlock17", ResidualBlock(filters, filters, filters));
        resBlock18 = register_module("resBlock18", ResidualBlock(filters, filters, filters));
        resBlock19 = register_module("resBlock19", ResidualBlock(filters, filters, filters));

        valueHead  = register_module("valueHead", ValueHead(filters, valueFilters, width, height, filters));
        policyHead = register_module("policyHead", PolicyHead(filters, policyFilters, width, height, outputs));
    }

    std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor const& input)
    {
        // the first convolutional layer
        auto x = convInput(input);
        // all residual blocks
        x = resBlock1(x);
        x = resBlock2(x);
        x = resBlock3(x);
        x = resBlock4(x);
        x = resBlock5(x);
        x = resBlock6(x);
        x = resBlock7(x);
        x = resBlock8(x);
        x = resBlock9(x);
        x = resBlock10(x);
        x = resBlock11(x);
        x = resBlock12(x);
        x = resBlock13(x);
        x = resBlock14(x);
        x = resBlock15(x);
        x = resBlock16(x);
        x = resBlock17(x);
        x = resBlock18(x);
        x = resBlock19(x);
        // return the two outputs
        return std::make_pair(policyHead(x), valueHead(x));
    }

    ConvBlock     convInput  = nullptr;
    ResidualBlock resBlock1  = nullptr;
    ResidualBlock resBlock2  = nullptr;
    ResidualBlock resBlock3  = nullptr;
    ResidualBlock resBlock4  = nullptr;
    ResidualBlock resBlock5  = nullptr;
    ResidualBlock resBlock6  = nullptr;
    ResidualBlock resBlock7  = nullptr;
    ResidualBlock resBlock8  = nullptr;
    ResidualBlock resBlock9  = nullptr;
    ResidualBlock resBlock10 = nullptr;
    ResidualBlock resBlock11 = nullptr;
    ResidualBlock resBlock12 = nullptr;
    ResidualBlock resBlock13 = nullptr;
    ResidualBlock resBlock14 = nullptr;
    ResidualBlock resBlock15 = nullptr;
    ResidualBlock resBlock16 = nullptr;
    ResidualBlock resBlock17 = nullptr;
    ResidualBlock resBlock18 = nullptr;
    ResidualBlock resBlock19 = nullptr;

    PolicyHead policyHead = nullptr;
    ValueHead  valueHead  = nullptr;
};

TORCH_MODULE(Network);