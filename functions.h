#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Include necessary libraries
#include <iostream>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"

// Function declarations
int features_7x7(cv::Mat &src, std::vector<float> &features);
int sum_squared_diff_topN(std::vector<float> &features, std::vector<std::vector<float>> &data, std::vector<char *> filenames, int N, std::vector<char *> &topN);
int features_DNN(char *target, std::vector<float> &features, std::vector<std::vector<float>> &data, std::vector<char *> filenames);
int cosine_similarity_topN(std::vector<float> &features, std::vector<std::vector<float>> &data, std::vector<char *> filenames, int N, std::vector<char *> &topN);

#endif