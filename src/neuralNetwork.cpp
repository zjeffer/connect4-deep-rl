#include "neuralNetwork.hpp"

NeuralNetwork::NeuralNetwork() {

}

NeuralNetwork::~NeuralNetwork() {

}

std::tuple<torch::Tensor, torch::Tensor> NeuralNetwork::predict(torch::Tensor &input) {
	return net->forward(input);
}

bool NeuralNetwork::loadModel(std::string path) {
    try {
        // load model from path
        LOG(INFO) << "Loading model from: " << path;
        torch::load(this->net, path);
    } catch (const std::exception& e) {
        LOG(WARNING) << "Error loading model: " << e.what();
        return false;
    }
    return true;
}

bool NeuralNetwork::saveModel(std::string path, bool isTrained){
    try {
        if (path.size() == 0) {
            path = "./models/model_" + utils::getTimeString();
        }
        if (isTrained) {
            path.append("_trained");
        }
        if (!path.ends_with(".pt")) {
            path.append(".pt");
        }
        if (std::filesystem::exists(path)) {
            LOG(WARNING) << "Model already exists at: " << path << ". Overwriting...";
            std::filesystem::remove(path);
        }
        // save model to path
        LOG(INFO) << "Saving model to: " << path;
		torch::save(this->net, path);
    } catch (const std::exception& e) {
        LOG(WARNING) << "Error saving model: " << e.what();
        return false;
    }
    return true;
}