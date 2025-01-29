#include <iostream>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"


int features_7x7(cv::Mat &src, std::vector<float> features) {
    int rows_center = src.rows / 2;
    int cols_center = src.cols / 2;

    int rows_start = rows_center - 3;
    int cols_start = cols_center - 3;

    int rows_end = rows_center + 3;
    int cols_end = cols_center + 3;

    for(int i = rows_start; i < rows_end; i++) {
        for(int j = cols_start; j < cols_end; j++) {
            for (int k = 0; k < 3; k++) {
                std::cout << src.at<cv::Vec3b>(i, j)[k] << " ";
                features.emplace_back(src.at<cv::Vec3b>(i, j)[k]);
            }
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <csv file>" << std::endl;
        return -1;
    }
    
    cv::Mat src = cv::imread("../sources/test_subset/pic.0001.jpg");
    std::vector<float> features;

    if(src.empty()) {
        std::cout << "Unable to open image" << std::endl;
        return -1;
    }

    features_7x7(src, features);
    for (int i = 0; i < features.size(); i++) {
        std::cout << features[i] << " ";
    }


    return 0;
}