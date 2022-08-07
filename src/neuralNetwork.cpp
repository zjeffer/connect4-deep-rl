#include "neuralNetwork.hpp"

NeuralNetwork::NeuralNetwork(Settings* settings) {
    m_Settings = settings;
    m_Net = Network(settings->getInputPlanes(), settings->getCols(), settings->getRows(), settings->getOutputSize(), 256, 2, 1);
    if (m_Settings->useGPU()){
        m_Device = torch::Device(torch::kCUDA);
    }
    m_Net->to(m_Device);
}

NeuralNetwork::~NeuralNetwork() {
    std::cout << "NeuralNetwork destructor" << std::endl;
}

torch::Tensor NeuralNetwork::boardToInput(Environment* env) {
    // Create input tensor
    torch::Tensor input = torch::zeros({m_Settings->getInputPlanes(), m_Settings->getRows(), m_Settings->getCols()});
	torch::Tensor board = env->getBoard();

    // Fill first plane of input tensor with board
    input[0] = board.detach().clone();

    // Fill second plane of input tensor with player (yellow = 0, red = 1)
    int player = static_cast<int>(env->getCurrentPlayer()) - 1;
    input[1] = torch::full({m_Settings->getRows(), m_Settings->getCols()}, player);

    return input.to(m_Device).unsqueeze(0);
}

std::tuple<torch::Tensor, torch::Tensor> NeuralNetwork::predict(torch::Tensor &input) {
	return m_Net->forward(input);
}

bool NeuralNetwork::loadModel(std::string path) {
    try {
        // load model from path
        LOG(INFO) << "Loading model from: " << path;
        torch::load(this->m_Net, path);
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
		torch::save(this->m_Net, path);
    } catch (const std::exception& e) {
        LOG(WARNING) << "Error saving model: " << e.what();
        return false;
    }
    return true;
}