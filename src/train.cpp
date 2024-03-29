#include "train.hpp"

#include <sstream>

#include "utils/utils.hpp"

Trainer::Trainer(std::shared_ptr<Settings> settings)
  : m_Settings(settings)
{
    // if useCUDA is false, it will use the CPU instead.
    if (m_Settings->useCUDA())
    {
        m_Device = torch::Device(torch::kCUDA);
    }

    // load the neural network based on the given settings
    m_NN = std::make_unique<NeuralNetwork>(m_Settings);
}

Trainer::~Trainer()
{
    LDEBUG << "Trainer destructor" << std::endl;
}

std::tuple<torch::Tensor, torch::Tensor> loss_function(std::tuple<torch::Tensor, torch::Tensor> const & outputs, torch::Tensor const & target)
{
    torch::Tensor policy_output = std::get<0>(outputs);
    torch::Tensor value_output  = std::get<1>(outputs);

    int policySize = policy_output.size(1);
    int valueSize  = 1;

    // divide policy and value targets
    int  size          = policy_output.size(0);
    auto policy_target = target.slice(1, 0, policySize).view({size, policySize});
    auto value_target  = target.slice(1, policySize, policySize + valueSize).view({size, valueSize});

    if (!torch::nan_to_num(policy_output).equal(policy_output))
    {
        LWARN << policy_output;
        LFATAL << "Policy output contains nans";
    }

    policy_output = policy_output.view({size, policySize});
    value_output  = value_output.view({size, valueSize});

    // loss
    torch::Tensor policy_loss = -torch::log_softmax(policy_output, 1).mul(policy_target).sum(1).mean();
    torch::Tensor value_loss  = torch::mse_loss(value_output, value_target);

    return std::make_tuple(policy_loss, value_loss);
}

std::filesystem::path Trainer::train()
{
    // load data
    C4Dataset dataset    = C4Dataset(m_Settings.get());
    int       batch_size = m_Settings->getBatchSize();

    // create data loader
    auto train_set      = dataset.map(torch::data::transforms::Stack<>());
    int  train_set_size = train_set.size().value();
    auto data_loader    = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(std::move(train_set), batch_size);

    // optimizer
    Network            net = m_NN->getNetwork();
    torch::optim::Adam optimizer(net->parameters(), m_Settings->getLearningRate());
    optimizer.zero_grad();

    // enable training mode
    m_NN->getNetwork()->train();
    m_NN->getNetwork()->to(m_Device);

    LossHistory loss_history;
    loss_history.batch_size = batch_size;
    loss_history.data_size  = train_set_size;

    LINFO << "Starting training with " << train_set_size << " samples. Learning rate: " << m_Settings->getLearningRate();
    for (int epoch = 0; epoch < m_Settings->getEpochs(); epoch++)
    {
        LINFO << "\n\nEpoch " << epoch << ": ";

        // TODO: plot loss
        float Loss = 0.0f;
        float Acc  = 0.0f;

        int index = 0;
        for (auto batch: *data_loader)
        {
            if (!g_Running)
            {
                exit(EXIT_FAILURE);
            }

            optimizer.zero_grad();
            int  size   = batch.data.sizes()[0];
            auto input  = batch.data.to(m_Device);
            auto target = batch.target.to(m_Device);

            // check if policy_target contains nans
            if (!torch::nan_to_num(target).equal(target))
            {
                LWARN << target;
                LFATAL << "Target contains nans";
            }

            std::tuple<torch::Tensor, torch::Tensor> predictions = m_NN->predict(input);
            std::tuple<torch::Tensor, torch::Tensor> losses      = loss_function(predictions, target);

            torch::Tensor policy_loss = std::get<0>(losses);
            torch::Tensor value_loss  = std::get<1>(losses);
            torch::Tensor loss        = policy_loss + value_loss;

            loss.backward();
            optimizer.step();

            // calculate average loss
            Loss += loss.item<float>();
            auto end = std::min(train_set_size, (index + 1) * batch_size);

            // calculate average accuracy of value output
            int           policySize    = std::get<0>(predictions).size(1);
            torch::Tensor value_output  = std::get<1>(predictions);
            torch::Tensor value_target  = target.slice(1, policySize, policySize + 1);
            int           amountCorrect = 0;
            for (int i = 0; i < value_output.size(0); i++)
            {
                if (std::abs(value_output[i].item<float>() - value_target[i].item<float>()) < 0.5)
                {
                    amountCorrect++;
                }
            }
            // calculate average accuracy
            Acc += (float)amountCorrect / (float)size;

            LINFO << "Epoch: " << epoch << ": Batch index: " << index << " => Average loss: " << Loss / end << ". Policy loss: " << policy_loss.item<float>()
                  << ". Value loss: " << value_loss.item<float>() << ". Value accuracy: " << Acc / (index + 1);

            // add loss to history
            loss_history.losses.push_back(loss.item<float>());
            loss_history.policies.push_back(policy_loss.item<float>());
            loss_history.values.push_back(value_loss.item<float>());
            loss_history.historySize++;

            index++;
        }
    }

    LINFO << "Training finished. Saving model...";

    std::string        timeString = utils::getTimeString();
    std::ostringstream oss;
    oss << "./models/model_" << timeString << "_lr" << std::to_string(m_Settings->getLearningRate()) << "_bs" << std::to_string(m_Settings->getBatchSize())
        << ".pt";
    // save loss history to csv, to make graphs with
    utils::writeLossToCSV("losses/history_" + timeString + ".csv", loss_history);
    // save the trained model
    std::filesystem::path trainedModelName = m_NN->saveModel(oss.str());

    // set network back to evaluation mode
    m_NN->getNetwork()->eval();
    return trainedModelName;
}
