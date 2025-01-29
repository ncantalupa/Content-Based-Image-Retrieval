#include <iostream>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"


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

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <csv file>" << std::endl;
        return -1;
    }
    for (int idx = 1; idx <= 1107; ++idx) {
        std::stringstream ss;
        ss << "../sources/olympus/pic."
           << std::setw(4) << std::setfill('0') << idx
           << ".jpg";
        std::string filename = ss.str();

        cv::Mat src = cv::imread(filename);
        if (src.empty()) {
            std::cerr << "Unable to open image: " << filename << std::endl;
            continue;
        }

        std::vector<float> features;
        features_7x7(src, features);

        append_image_data_csv(argv[1], (char*)filename.c_str(), features, 0);
    }

    return 0;
}