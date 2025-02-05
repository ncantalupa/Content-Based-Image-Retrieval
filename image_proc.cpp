#include <iostream>
#include <string>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"
#include "functions.h"
#include "DA2Network.hpp"

int main(const int argc, char* argv[])
{
    // Ensure correct number of arguments
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " <images directory> <processing function> <csv file>" <<
            std::endl;
        return -1;
    }

    // Map argv[2] to the corresponding processing function
    std::function<int(char*, std::vector<float>&)> processing_func;
    get_feature_function(argv[2], processing_func);

    // Iterate through all jpg files in the directory specified by argv[1]
    for (const auto& entry : std::filesystem::directory_iterator(argv[1]))
    {
        if (entry.is_regular_file() && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png"))
        {
            std::string filename = entry.path().string();
            cv::Mat src = cv::imread(filename);
            char* img_fp = new char[filename.size() + 1];
            std::strcpy(img_fp, filename.c_str());

            // Skip and print warning if unable to open image
            if (src.empty())
            {
                std::cerr << "Unable to open image: " << filename << std::endl;
                continue;
            }

            // Calculate features and write to csv file given by argv[3]
            std::vector<float> features;
            processing_func(img_fp, features);
            append_image_data_csv(argv[3], const_cast<char*>(filename.c_str()), features, 0);
        }
    }

    return 0;
}


// int main(int argc, char* argv[]) {
//     if(argc < 2) {
//         std::cout << "Usage: " << argv[0] << " <csv file>" << std::endl;
//         return -1;
//     }
//     for (int idx = 1; idx <= 25; ++idx) {
//         std::stringstream ss;
//         // Olympus Pictures Loading
//         // ss << "../sources/olympus/pic."
//         //    << std::setw(4) << std::setfill('0') << idx
//         //    << ".jpg";
//         //    std::string filename = ss.str();

//         // Can Image Loading
//         ss << "../sources/can_pics/can_"
//            << std::setw(4) << std::setfill('0') << idx
//            << ".png";
//         std::string filename = ss.str();

//         cv::Mat src = cv::imread(filename);
//         if (src.empty()) {
//             std::cerr << "Unable to open image: " << filename << std::endl;
//             continue;
//         }

//         std::vector<float> features;
//         features_7x7(src, features);

//         cv::Mat temp;
//         src.copyTo(temp);
//         DA2Network da_net( "model_fp16.onnx" );
//         da_net.set_input(src);
//         da_net.run_network(temp, temp.size());
//         cv::imshow("src", src);
//         cv::imshow("DA2", temp);
//         cv::waitKey(0);


//         append_image_data_csv(argv[1], (char*)filename.c_str(), features, 0);
//     }

//     return 0;
// }