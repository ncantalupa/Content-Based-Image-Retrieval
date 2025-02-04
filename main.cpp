#include <iostream>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"
#include "functions.h"

int main(const int argc, char* argv[])
{
    if (argc < 6)
    {
        std::cout << "Usage: " << argv[0] <<
            " <target image file> <features csv file> <feature processing function> <distance metric> <number of similar images>"
            << std::endl;
        return -1;
    }
    // Use read_image_data_csv to read the CSV file
    std::vector<char*> filenames;
    std::vector<std::vector<float>> data;
    read_image_data_csv(argv[2], filenames, data, 0);
    // Use cv::imread to read the image file
    cv::Mat src = cv::imread(argv[1]);
    if (src.empty())
    {
        std::cout << "Unable to read image file" << std::endl;
        return -1;
    }

    // Map argv[3] to the corresponding processing function
    std::function<int(cv::Mat&, std::vector<float>&)> processing_func;
    get_feature_function(argv[3], processing_func);

    // Use processing_func to calculate features
    std::vector<float> features;
    processing_func(src, features);

    // Map argv[4] to the corresponding distance function
    std::function<float(const std::vector<float>&, const std::vector<float>&, float& distance)> distance_func;
    get_distance_function(argv[4], distance_func);

    // Get number of similar images from argv[5]
    const int n = std::stoi(argv[5]);

    // Get top N matches using distance metric
    std::vector<char*> topN;
    closest_n_images(features, data, distance_func, filenames, n, topN);

    // Print the top N matches filenames
    for (int i = 0; i < n; i++)
    {
        std::cout << topN[i] << std::endl;
        cv::Mat t = cv::imread(topN[i]);
        cv::imshow("Match " + std::to_string(i), t);
        cv::waitKey(0);
    }

    return 0;
}
