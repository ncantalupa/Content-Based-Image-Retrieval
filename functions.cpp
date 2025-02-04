#include <iostream>
#include <opencv2/opencv.hpp>
#include "functions.h"

int features_7x7(cv::Mat& src, std::vector<float>& features)
{
    const int rows_center = src.rows / 2;
    const int cols_center = src.cols / 2;

    const int rows_start = rows_center - 3;
    const int cols_start = cols_center - 3;

    const int rows_end = rows_center + 3;
    const int cols_end = cols_center + 3;

    for (int i = rows_start; i < rows_end; i++)
    {
        for (int j = cols_start; j < cols_end; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                features.push_back(src.at<cv::Vec3b>(i, j)[k]);
            }
        }
    }
    return 0;
}

int histogram_hs(const cv::Mat& src, std::vector<float>& features)
{
    // Convert image to HSV
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    // Define histogram size and initialize histogram
    constexpr int histsize = 16; // Don't set this much higher than 64
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hsv.cols; j++)
        {
            const int h = static_cast<int>(static_cast<float>(row[j][0]) / 256.0 * histsize);
            const int s = static_cast<int>(static_cast<float>(row[j][1]) / 256.0 * histsize);
            hist.at<float>(h, s) += 1.0;
        }
    }

    // Normalize histogram
    hist /= hsv.rows * hsv.cols;

    // Flatten histogram for csv output
    for (int i = 0; i < hist.rows; i++)
    {
        for (int j = 0; j < hist.cols; j++)
        {
            float value = hist.at<float>(i, j);
            features.push_back(value);
        }
    }

    return 0;
}

int histogram_hs_left(const cv::Mat& src, std::vector<float>& features)
{
    // Convert image to HSV
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    // Define histogram size and initialize histogram
    constexpr int histsize = 16; // Don't set this much higher than 64
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hsv.cols/2; j++)
        {
            const int h = static_cast<int>(static_cast<float>(row[j][0]) / 256.0 * histsize);
            const int s = static_cast<int>(static_cast<float>(row[j][1]) / 256.0 * histsize);
            hist.at<float>(h, s) += 1.0;
        }
    }

    // Normalize histogram
    hist /= hsv.rows * hsv.cols;

    // Flatten histogram for csv output
    for (int i = 0; i < hist.rows; i++)
    {
        for (int j = 0; j < hist.cols; j++)
        {
            float value = hist.at<float>(i, j);
            features.push_back(value);
        }
    }

    return 0;
}

int histogram_hs_vertical_center(const cv::Mat& src, std::vector<float>& features)
{
    // Convert image to HSV
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    // Define histogram size and initialize histogram
    constexpr int histsize = 16; // Don't set this much higher than 64
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = hsv.cols/4; j < 3*hsv.cols/4; j++)
        {
            const int h = static_cast<int>(static_cast<float>(row[j][0]) / 256.0 * histsize);
            const int s = static_cast<int>(static_cast<float>(row[j][1]) / 256.0 * histsize);
            hist.at<float>(h, s) += 1.0;
        }
    }

    // Normalize histogram
    hist /= hsv.rows * hsv.cols;

    // Flatten histogram for csv output
    for (int i = 0; i < hist.rows; i++)
    {
        for (int j = 0; j < hist.cols; j++)
        {
            float value = hist.at<float>(i, j);
            features.push_back(value);
        }
    }

    return 0;
}


int histogram_hs_right(const cv::Mat& src, std::vector<float>& features)
{
    // Convert image to HSV
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    // Define histogram size and initialize histogram
    constexpr int histsize = 16; // Don't set this much higher than 64
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = hsv.cols/2; j < hsv.cols; j++)
        {
            const int h = static_cast<int>(static_cast<float>(row[j][0]) / 256.0 * histsize);
            const int s = static_cast<int>(static_cast<float>(row[j][1]) / 256.0 * histsize);
            hist.at<float>(h, s) += 1.0;
        }
    }

    // Normalize histogram
    hist /= hsv.rows * hsv.cols;

    // Flatten histogram for csv output
    for (int i = 0; i < hist.rows; i++)
    {
        for (int j = 0; j < hist.cols; j++)
        {
            float value = hist.at<float>(i, j);
            features.push_back(value);
        }
    }

    return 0;
}

int multihistogram_hs(const cv::Mat& src, std::vector<float>& features)
{
    // Calculate HS histograms for the left half, right half, and vertical center half of the image
    std::vector<float> left_features;
    std::vector<float> right_features;
    std::vector<float> center_features;
    histogram_hs_left(src, left_features);
    histogram_hs_right(src, right_features);
    histogram_hs_vertical_center(src, center_features);

    // Append the three histogram feature vectors to the output features vector
    features.insert(features.end(), left_features.begin(), left_features.end());
    features.insert(features.end(), right_features.begin(), right_features.end());
    features.insert(features.end(), center_features.begin(), center_features.end());

    return 0;
}

int closest_n_images(const std::vector<float>& features, const std::vector<std::vector<float>>& data,
                     const std::function<float(const std::vector<float>&, const std::vector<float>&, float& distance)>&
                     distance_function,
                     const std::vector<char*>& filenames, const int n, std::vector<char*>& topN)
{
    std::vector<float> ssd;
    for (const auto& i : data)
    {
        float distance;
        distance_function(features, i, distance);
        ssd.push_back(distance);
    }

    for (int i = 0; i < n; i++)
    {
        int min_index = 0;
        float min_value = ssd[0];
        for (int j = 1; j < ssd.size(); j++)
        {
            if (ssd[j] < min_value)
            {
                min_value = ssd[j];
                min_index = j;
            }
        }
        topN.push_back(filenames[min_index]);
        ssd[min_index] = INT_MAX;
    }
    return 0;
}

int sum_squared_difference(const std::vector<float>& features1, const std::vector<float>& features2, float& distance)
{
    float sum = 0;
    for (int i = 0; i < features1.size(); i++)
    {
        sum += (features1[i] - features2[i]) * (features1[i] - features2[i]);
    }
    distance = sum;
    return 0;
}

int sum_absolute_difference(const std::vector<float>& features1, const std::vector<float>& features2, float& distance)
{
    float sum = 0;
    for (int i = 0; i < features1.size(); i++)
    {
        sum += std::abs(features1[i] - features2[i]);
    }
    distance = sum;
    return 0;
}

int histogram_intersection(const std::vector<float>& features1, const std::vector<float>& features2, float& distance)
{
    float sum = 0;
    for (int i = 0; i < features1.size(); i++)
    {
        sum += std::min(features1[i], features2[i]);
    }

    // Convert sum to distance (complete intersection would result in a sum of 1, which should be a distance of 0)
    distance = 1 - sum;

    return 0;
}


int get_feature_function(const char* function_name, std::function<int(cv::Mat&, std::vector<float>&)>& processing_func)
{
    std::map<std::string, std::function<int(cv::Mat&, std::vector<float>&)>> processing_functions = {
        // Add processing functions here
        {"baseline", features_7x7},
        {"histogram", histogram_hs}
    };
    if (const auto iterator = processing_functions.find(function_name); iterator != processing_functions.end())
    {
        processing_func = iterator->second;
    }
    else
    {
        std::cerr << "Unknown processing function: " << function_name << std::endl;
        return -1;
    }
    return 0;
}

int get_distance_function(const char* function_name,
                          std::function<float(const std::vector<float>&, const std::vector<float>&, float& distance)>&
                          distance_func)
{
    std::map<std::string, std::function<float(const std::vector<float>&, const std::vector<float>&, float& distance)>>
        processing_functions = {
            // Add distance functions here
            {"ssd", sum_squared_difference},
            {"hist_intersection", histogram_intersection}
        };
    if (const auto iterator = processing_functions.find(function_name); iterator != processing_functions.end())
    {
        distance_func = iterator->second;
    }
    else
    {
        std::cerr << "Unknown distance function: " << function_name << std::endl;
        return -1;
    }
    return 0;
}
