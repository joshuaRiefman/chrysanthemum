//
// Created by Joshua Riefman on 2023-02-20.
//

#include "../include/helpers.h"

int helpers::MaxInArray(std::vector<int>& array) {
    int max = 0;

    for (int i = 0; i < array.size(); i++) {
        if (array[i] > max) { max = array[i]; }
    }

    return max;
}

double helpers::ReLU(double value) {
    return value > 0 ? value : 0;
}

int helpers::Sum(std::vector<int>& array) {
    int result = 0;

    for (int i = 0; i < array.size(); i++) {
        result += array[i];
    }

    return result;
}

double helpers::GetRandomNormalized() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(-1000, 1000);
    return (float)distribution(gen)/1000;
}

long helpers::GetDuration(const std::chrono::time_point<std::chrono::system_clock> &start) {
    auto end = std::chrono::system_clock::now();
    time_t end_time = std::chrono::system_clock::to_time_t(end);
    time_t start_time = std::chrono::system_clock::to_time_t(start);
    long elapsed_seconds = end_time - start_time;
    return elapsed_seconds;
}