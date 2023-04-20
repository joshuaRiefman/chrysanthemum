#include "../include/jsoncpp/json/json.h"
#include "Neuron.h"
#include "Layer.h"
#include "helpers.h"
#include "NeuralNetwork.h"
#include "Chrysanthemum.h"

std::vector<float> &GetArrayFromJSON(int planetID, const Json::Value *data, std::vector<float> &distances, const std::string &fieldAccessor) {
    for (int i = 0; i < universeSize; ++i) {
        distances[i] = (*data)["Planets"][planetID][fieldAccessor][i].asFloat();
    }
    return distances;
}

City ParseCityData(int cityID, int citiesCount) {
    std::ifstream filePath(DataJSONPath);
    Json::Reader reader;
    Json::Value data;

    reader.parse(filePath, data);
    filePath.close();

    int id = cityID;
    double distanceFromOrigin = data["distance_from_origin"].asDouble();

    std::vector<float> distances;
    distances.resize(citiesCount);
    distances = GetArrayFromJSON(cityID, &data, distances, "distances");

    std::vector<float> deltaDistances;
    deltaDistances.resize(citiesCount);
    deltaDistances = GetArrayFromJSON(cityID, &data, deltaDistances, "deltaDistances");

    City newCity(id, distances, distanceFromOrigin, deltaDistances, false);
    return newCity;
}

void UpdateUniverseConstants() {
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "    ";

    Json::Value value;
    value["universeSize"] = (int)universeSize;

    std::ofstream outputFileStream(ConstantsJSONPath);
    builder.newStreamWriter()->write(value, &outputFileStream);

    outputFileStream.close();
}

void SetOrigin() {
    std::ifstream filePath(DataJSONPath);
    Json::Reader reader;
    Json::Value data;

    reader.parse(filePath, data);
    filePath.close();

    originCityID = data["starting_position"].asInt();
    cities[originCityID].visited = true;
}

void InitializePlanets(int citiesCount) {
    cities.resize(citiesCount);
    for (int i = 0; i < citiesCount; i++)
    {
        cities[i] = ParseCityData(i, citiesCount);
    }

    SetOrigin();
}

static InputLayer SetNetworkInputs(std::vector<int> *planetIDList) {
    std::vector<double> outputs;
    for (int i = 0; i < universeSize; ++i) {

        if (cities[i].visited) {
            outputs.emplace_back(1);
        } else { outputs.emplace_back(0); }

        planetIDList->emplace_back(cities[i].id);

        for (int j = 0; j < cities[i].distances.size(); ++j) {
            outputs.emplace_back(cities[i].distances[j]);
        }
//        for (int j = 0; j < cities[i].deltaDistances.size(); ++j) {
//            outputs.emplace_back(cities[i].deltaDistances[j]);
//        }
    }

    return InputLayer(&outputs);
}

void InitializeWorld() {
    UpdateUniverseConstants();
    InitializePlanets(universeSize);
}

NeuralNetworkConfiguration CreateConfig(int numOutputs) {
    std::vector<int> planetIDList;
    InputLayer inputs = SetNetworkInputs(&planetIDList);
    std::vector<int> layerSizes = {2, 4, 4, 3};
    Eigen::Matrix<std::vector<double>, Eigen::Dynamic, Eigen::Dynamic> weights = GetRandomWeights(layerSizes, (int)layerSizes.size(), (int)inputs.outputs.size());
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> biases = GetRandomBiases(layerSizes, (int)layerSizes.size());

    NeuralNetworkConfiguration config = NeuralNetworkConfiguration(layerSizes, inputs, weights, biases, planetIDList, numOutputs);
    return config;
}

int main() {
    auto start = std::chrono::system_clock::now();

    InitializeWorld();

    NeuralNetworkConfiguration config = CreateConfig((int)cities.size());

    NeuralNetwork neuralNetwork = NeuralNetwork(&neuralNetwork, &config);

    NeuralNetwork::Solve(&neuralNetwork);

    for (int i = 0; i < neuralNetwork.layers[neuralNetwork.size-1].outputs.size(); i++) {
        std::cout << std::to_string(neuralNetwork.layers[neuralNetwork.size-1].outputs[i].activation) << std::endl;
    }

    int newPosition = Neuron::GetHighestNeuronActivationById(&neuralNetwork.layers[3].outputs); //Not getting correct layer
    //Cities should be put in as inputs in decreasing order in terms of distance
    std::cout << "New Position is: " + std::to_string(newPosition) << std::endl;

    long elapsed_seconds = helpers::GetDuration(start);

    std::cout << "Executed successfully in " + std::to_string(elapsed_seconds) + "s!\n";
}