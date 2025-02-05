#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"
#include "functions.h"
#include "DA2Network.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <csv file>" << std::endl;
        return -1;
    }
    for (int idx = 1; idx <= 25; ++idx) {
        std::stringstream ss;
        // Olympus Pictures Loading
        // ss << "../sources/olympus/pic."
        //    << std::setw(4) << std::setfill('0') << idx
        //    << ".jpg";
        //    std::string filename = ss.str();

        // Can Image Loading
        ss << "../sources/can_pics/can_"
           << std::setw(4) << std::setfill('0') << idx
           << ".png";
        std::string filename = ss.str();

        cv::Mat src = cv::imread(filename);
        if (src.empty()) {
            std::cerr << "Unable to open image: " << filename << std::endl;
            continue;
        }

        std::vector<float> features;
        features_7x7(src, features);

        cv::Mat temp;
        src.copyTo(temp);
        DA2Network da_net( "model_fp16.onnx" );
        da_net.set_input(src);
        da_net.run_network(temp, temp.size());
        cv::imshow("src", src);
        cv::imshow("DA2", temp);
        cv::waitKey(0);


        append_image_data_csv(argv[1], (char*)filename.c_str(), features, 0);
    }

    return 0;
}