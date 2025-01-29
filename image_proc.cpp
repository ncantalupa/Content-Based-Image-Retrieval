#include <iostream>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"
#include "functions.h"

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