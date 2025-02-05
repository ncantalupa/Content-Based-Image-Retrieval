#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"
#include "functions.h"
#include "DA2Network.hpp"

int features_7x7(cv::Mat &src, std::vector<float> &features) {
    int rows_center = src.rows / 2;
    int cols_center = src.cols / 2;

    int rows_start = rows_center - 3;
    int cols_start = cols_center - 3;

    int rows_end = rows_center + 3;
    int cols_end = cols_center + 3;

    for(int i = rows_start; i < rows_end; i++) {
        for(int j = cols_start; j < cols_end; j++) {
            for (int k = 0; k < 3; k++) {
                features.push_back((int)src.at<cv::Vec3b>(i, j)[k]);
            }
        }
    }
    return 0;
}

int sum_squared_diff_topN(std::vector<float> &features, std::vector<std::vector<float>> &data, std::vector<char *> filenames, int N, std::vector<char *> &topN) {
    std::vector<float> ssd;
    for(int i = 0; i < data.size(); i++) {
        float sum = 0;
        for(int j = 0; j < features.size(); j++) {
            sum += (features[j] - data[i][j]) * (features[j] - data[i][j]);
        }
        ssd.push_back(sum);
    }

    for(int i = 0; i < N; i++) {
        int min_index = 0;
        float min_value = ssd[0];
        for(int j = 1; j < ssd.size(); j++) {
            if(ssd[j] < min_value) {
                min_value = ssd[j];
                min_index = j;
            }
        }
        topN.push_back(filenames[min_index]);
        ssd[min_index] = INT_MAX;
    }
    return 0;
}

int features_DNN(char *target, std::vector<float> &features, std::vector<std::vector<float>> &data, std::vector<char *> filenames) {
    // Set features to be row from data with name target
    for(int i = 0; i < filenames.size(); i++) {
        if(strcmp(filenames[i], target) == 0) {
            for (int j = 0; j < data[i].size(); j++) {
                features.push_back(data[i][j]);
            }
            break;
        }
    }
    return 0;
}

int cosine_similarity_topN(std::vector<float> &features, std::vector<std::vector<float>> &data, std::vector<char *> filenames, int N, std::vector<char *> &topN) {
    std::vector<float> cos_sim;
    for(int i = 0; i < data.size(); i++) {
        float dot_product = 0;
        float norm_features = 0;
        float norm_data = 0;
        for(int j = 0; j < features.size(); j++) {
            dot_product += features[j] * data[i][j];
            norm_features += features[j] * features[j];
            norm_data += data[i][j] * data[i][j];
        }
        cos_sim.push_back(dot_product / (sqrt(norm_features) * sqrt(norm_data)));
    }

    for(int i = 0; i < N; i++) {
        int max_index = 0;
        float max_value = cos_sim[0];
        for(int j = 1; j < cos_sim.size(); j++) {
            if(cos_sim[j] > max_value) {
                max_value = cos_sim[j];
                max_index = j;
            }
        }
        topN.push_back(filenames[max_index]);
        cos_sim[max_index] = -1;
    }
    return 0;
}

int features_DA_cans(cv::Mat &src, std::vector<float> &features) {

    return 0;
}