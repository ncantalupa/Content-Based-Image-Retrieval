#include <iostream>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    if(argc < 3) {
        std::cout << "Usage: " << argv[0] << " <csv file> <image file>" << std::endl;
    }
    // Use read_image_data_csv to read the CSV file
    std::vector<char *> filenames;
    std::vector<std::vector<float>> data;
    read_image_data_csv(argv[1], filenames, data, 0);
    // Use cv::imread to read the image file
    cv::Mat src = cv::imread(argv[2]);
    if(src.empty()) {
        std::cout << "Unable to read image file" << std::endl;
        return -1;
    }
    // Use features_7x7 to extract features from the image
    std::vector<float> features;
    features_7x7(src, features);

    // Get top N matches using sum_squared_diff
    int N = 5;
    std::vector<char *> topN;
    sum_squared_diff_topN(features, data, filenames, N, topN);
    // Print the top N matches filenames
    for(int i = 0; i < N; i++) {
        std::cout << topN[i] << std::endl;
        cv::Mat t = cv::imread(topN[i]);
        cv::imshow("Match " + std::to_string(i), t);
        cv::waitKey(0);
    }
    
    return 0;
}