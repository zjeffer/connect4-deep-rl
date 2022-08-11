#include "neuralNetwork.hpp"

NeuralNetwork::NeuralNetwork(Settings* selfPlaySettings) {
    m_Settings = selfPlaySettings;
    m_Net = Network(selfPlaySettings->getInputPlanes(), selfPlaySettings->getRows(), selfPlaySettings->getCols(), selfPlaySettings->getOutputSize(), 256, 2, 1);
    if (m_Settings->useGPU()){
        m_Device = torch::Device(torch::kCUDA);
    }
    m_Net->to(m_Device);
}

NeuralNetwork::~NeuralNetwork() {
    // std::cout << "NeuralNetwork destructor" << std::endl;
}

Network NeuralNetwork::getNetwork(){
    return m_Net;
}

torch::Tensor NeuralNetwork::boardToInput(torch::Tensor board, int player, int inputPlanes) {
    // Create input tensor
    int rows = board.size(0);
    int cols = board.size(1);
    torch::Tensor input = torch::zeros({inputPlanes, rows, cols});

    // 2 planes for pieces, 1 plane for player
    // input[0] is the plane with yellow pieces
    // input[1] is the plane with red pieces
    // input[2] is the plane filled with 1 if the player is yellow, 0 otherwise
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j].item<int>() == 1) {
                input[0][i][j] = 1;
            } else if (board[i][j].item<int>() == 2) {
                input[1][i][j] = 1;
            }
        }
    }
    input[2] = torch::full({rows, cols}, player);

    // LOG(DEBUG) << "input created";
    return input.unsqueeze(0);
}

torch::Tensor NeuralNetwork::boardToInput(Environment* env) {
    return NeuralNetwork::boardToInput(env->getBoard().detach().clone(), static_cast<int>(env->getCurrentPlayer()), m_Settings->getInputPlanes()).to(m_Device);
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
		torch::save(this->m_Net, path);
        LOG(INFO) << "Saved model to: " << path;
    } catch (const std::exception& e) {
        LOG(WARNING) << "Error saving model: " << e.what();
        return false;
    }
    return true;
}