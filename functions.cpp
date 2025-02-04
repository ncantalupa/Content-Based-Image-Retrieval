#include <iostream>
#include <opencv2/opencv.hpp>
#include "functions.h"

int HIST_SIZE = 16; // Don't set too high, and ensure it's used in all histogram functions and distance metrics

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
    const int histsize = HIST_SIZE;
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
    const int histsize = HIST_SIZE;
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hsv.cols / 2; j++)
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
    const int histsize = HIST_SIZE;
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = hsv.cols / 4; j < 3 * hsv.cols / 4; j++)
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
    const int histsize = HIST_SIZE;
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = hsv.cols / 2; j < hsv.cols; j++)
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

int histogram_texture(const cv::Mat& src, std::vector<float>& features)
{
    // Filter image
    cv::Mat sx;
    cv::Mat sy;
    cv::Mat mag = src.clone();
    sobelX3x3(src, sx);
    sobelY3x3(src, sy);
    magnitude(sx, sy, mag);

    // Define histogram size and initialize histogram
    const int histsize = HIST_SIZE;
    cv::Mat hist = cv::Mat::zeros(cv::Size(histsize, histsize),CV_32FC1);

    // Calculate histogram for HS components
    for (int i = 0; i < mag.rows; i++)
    {
        auto* row = mag.ptr<cv::Vec3b>(i);
        for (int j = 0; j < mag.cols; j++)
        {
            const int h = static_cast<int>(static_cast<float>(row[j][0]) / 256.0 * histsize);
            const int s = static_cast<int>(static_cast<float>(row[j][1]) / 256.0 * histsize);
            hist.at<float>(h, s) += 1.0;
        }
    }

    // Normalize histogram
    hist /= mag.rows * mag.cols;

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


int texture_and_color(const cv::Mat& src, std::vector<float>& features)
{
    // Calculate histograms
    std::vector<float> texture_features;
    std::vector<float> color_features;
    histogram_texture(src, texture_features);
    histogram_hs(src, color_features);

    // Append histogram features to the output features vector
    features.insert(features.end(), texture_features.begin(), texture_features.end());
    features.insert(features.end(), color_features.begin(), color_features.end());

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

int multihistogram_difference(const std::vector<float>& features1, const std::vector<float>& features2, float& distance)
{
    // Split features into left, right, and center histogram features
    const int n_per_histogram = HIST_SIZE * HIST_SIZE; // Should match histsize in histogram functions
    const std::vector<float> left1(features1.begin(), features1.begin() + n_per_histogram);
    const std::vector<float> right1(features1.begin() + n_per_histogram, features1.begin() + 2 * n_per_histogram);
    const std::vector<float> center1(features1.begin() + 2 * n_per_histogram, features1.end());
    const std::vector<float> left2(features2.begin(), features2.begin() + n_per_histogram);
    const std::vector<float> right2(features2.begin() + n_per_histogram, features2.begin() + 2 * n_per_histogram);
    const std::vector<float> center2(features2.begin() + 2 * n_per_histogram, features2.end());

    // Calculate distances between the three histograms
    float left_distance;
    float right_distance;
    float center_distance;
    sum_absolute_difference(left1, left2, left_distance);
    sum_absolute_difference(right1, right2, right_distance);
    sum_absolute_difference(center1, center2, center_distance);

    // Combine distance metrics
    distance = 0.15f * left_distance + 0.15f * right_distance + 0.7f * center_distance;

    return 0;
}

int texture_and_color_difference(const std::vector<float>& features1, const std::vector<float>& features2,
                                 float& distance)
{
    // Split features into texture and color features (texture features are first)
    const int n_per_histogram = HIST_SIZE * HIST_SIZE; // Should match histsize in histogram functions
    const std::vector<float> texture1(features1.begin(), features1.begin() + n_per_histogram);
    const std::vector<float> color1(features1.begin() + n_per_histogram, features1.end());
    const std::vector<float> texture2(features2.begin(), features2.begin() + n_per_histogram);
    const std::vector<float> color2(features2.begin() + n_per_histogram, features2.end());

    // Calculate distances between the two histograms
    float texture_distance;
    float color_distance;
    histogram_intersection(texture1, texture2, texture_distance);
    histogram_intersection(color1, color2, color_distance);

    // Combine distance metrics
    distance = 0.5f * texture_distance + 0.5f * color_distance;

    return 0;
}


int get_feature_function(const char* function_name, std::function<int(cv::Mat&, std::vector<float>&)>& processing_func)
{
    std::map<std::string, std::function<int(cv::Mat&, std::vector<float>&)>> processing_functions = {
        // Add processing functions here
        {"baseline", features_7x7},
        {"histogram", histogram_hs},
        {"multihistogram", multihistogram_hs},
        {"texture_color", texture_and_color}
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
            {"hist_intersection", histogram_intersection},
            {"multihistogram_diff", multihistogram_difference},
            {"texture_color_diff", texture_and_color_difference}
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

/*
 *Applies a 3 by 3 Sobel filter to an image to find the horizontal edges.
 *
 *Kernel:
 *[ 1  2  1
 *  0  0  0
 * -1 -2  -1]
 *
 *Arguments:
 *cv::Mat &src: input image in BGR
 *cv::Mar &dst: output image in BGR with horizontal edges highlighted in 16SC3 (signed short format)
 */
int sobelX3x3(const cv::Mat& src, cv::Mat& dst)
{
    // Defines separable kernels
    constexpr int kernelY[3] = {1, 2, 1};
    constexpr int kernelX[3] = {-1, 0, 1};
    dst = cv::Mat::zeros(src.size(), CV_16SC3); // Define output image with 16-bit signed short format
    cv::Mat intermediate = dst.clone(); // intermediate image to store data between passes

    // Vertical pass
    for (int i = 1; i < src.rows - 1; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            // Calculate weighted sum of each row per channel
            int sumB = 0, sumG = 0, sumR = 0;
            for (int k = 0; k < 3; k++)
            {
                const auto* srcRow = src.ptr<uchar>(i - 1 + k);
                sumB += srcRow[j * 3] * kernelY[k];
                sumG += srcRow[j * 3 + 1] * kernelY[k];
                sumR += srcRow[j * 3 + 2] * kernelY[k];
            }
            // Reassigns pixel values to sum per channel
            auto* interRow = intermediate.ptr<short>(i);
            interRow[j * 3] = static_cast<short>(sumB);
            interRow[j * 3 + 1] = static_cast<short>(sumG);
            interRow[j * 3 + 2] = static_cast<short>(sumR);
        }
    }

    // Horizontal pass
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 1; j < src.cols - 1; j++)
        {
            // Calculate weighted sum of each column per channel
            int sumB = 0, sumG = 0, sumR = 0;
            for (int k = 0; k < 3; k++)
            {
                const short* interRow = intermediate.ptr<short>(i);
                sumB += interRow[(j - 1 + k) * 3] * kernelX[k];
                sumG += interRow[(j - 1 + k) * 3 + 1] * kernelX[k];
                sumR += interRow[(j - 1 + k) * 3 + 2] * kernelX[k];
            }
            // Reassigns pixel values to sum per channel
            auto* dstRow = dst.ptr<short>(i);
            dstRow[j * 3] = static_cast<short>(sumB);
            dstRow[j * 3 + 1] = static_cast<short>(sumG);
            dstRow[j * 3 + 2] = static_cast<short>(sumR);
        }
    }

    return 0;
}

/*
 *Applies a 3 by 3 Sobel filter to an image to find the horizontal edges.
 *
 *Kernel:
 *[-1  0  1
 * -2  0  2
 * -1  0  1]
 *
 *Arguments:
 *cv::Mat &src: input image in BGR
 *cv::Mar &dst: output image in BGR with horizontal edges highlighted in 16SC3 (signed short format)
 */
int sobelY3x3(const cv::Mat& src, cv::Mat& dst)
{
    constexpr int kernelY[3] = {-1, 0, 1};
    constexpr int kernelX[3] = {1, 2, 1};
    dst = cv::Mat::zeros(src.size(), CV_16SC3); // Define output image with 16-bit signed short format
    cv::Mat intermediate = dst.clone(); // intermediate image to store data between passes

    // Vertical pass
    for (int i = 1; i < src.rows - 1; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            // Calculate weighted sum of each row per channel
            int sumB = 0, sumG = 0, sumR = 0;
            for (int k = 0; k < 3; k++)
            {
                const auto* srcRow = src.ptr<uchar>(i - 1 + k);
                sumB += srcRow[j * 3] * kernelY[k];
                sumG += srcRow[j * 3 + 1] * kernelY[k];
                sumR += srcRow[j * 3 + 2] * kernelY[k];
            }
            // Reassigns pixel values to sum per channel
            auto* interRow = intermediate.ptr<short>(i);
            interRow[j * 3] = static_cast<short>(sumB);
            interRow[j * 3 + 1] = static_cast<short>(sumG);
            interRow[j * 3 + 2] = static_cast<short>(sumR);
        }
    }

    // Horizontal pass
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 1; j < src.cols - 1; j++)
        {
            int sumB = 0, sumG = 0, sumR = 0;
            for (int k = 0; k < 3; k++)
            {
                // Calculate weighted sum of each column per channel
                const short* interRow = intermediate.ptr<short>(i);
                sumB += interRow[(j - 1 + k) * 3] * kernelX[k];
                sumG += interRow[(j - 1 + k) * 3 + 1] * kernelX[k];
                sumR += interRow[(j - 1 + k) * 3 + 2] * kernelX[k];
            }
            // Reassigns pixel values to sum per channel
            auto* dstRow = dst.ptr<short>(i);
            dstRow[j * 3] = static_cast<short>(sumB);
            dstRow[j * 3 + 1] = static_cast<short>(sumG);
            dstRow[j * 3 + 2] = static_cast<short>(sumR);
        }
    }

    return 0;
}

/*
 *Calculates the magnitude of the gradient of an image using the Sobel filter.
 *Detects edges.
 *
 *Arguments:
 *cv::Mat &sx: image in BGR with horizontal edges highlighted in 16SC3 (signed short format)
 *cv::Mat &sy: image in BGR with vertical edges highlighted in 16SC3 (signed short format)
 *cv::Mar &dst: output image in BGR 8UC3 with the magnitude of the gradient
 */
int magnitude(const cv::Mat& sx, cv::Mat& sy, cv::Mat& dst)
{
    // TODO: Fix issue where passing empty dst matrix causes a crash
    for (int i = 0; i < sx.rows; i++)
    {
        for (int j = 0; j < sx.cols; j++)
        {
            // Iterates through each pixel and reassigns to the Euclidian norm of the Sobel X and Y values
            const auto sxRow = sx.ptr<short>(i);
            const auto syRow = sy.ptr<short>(i);
            const auto dstRow = dst.ptr<uchar>(i);
            dstRow[j * 3] = static_cast<uchar>(std::sqrt(sxRow[j * 3] * sxRow[j * 3] + syRow[j * 3] * syRow[j * 3]));
            dstRow[j * 3 + 1] = static_cast<uchar>(std::sqrt(
                sxRow[j * 3 + 1] * sxRow[j * 3 + 1] + syRow[j * 3 + 1] * syRow[j * 3 + 1]));
            dstRow[j * 3 + 2] = static_cast<uchar>(std::sqrt(
                sxRow[j * 3 + 2] * sxRow[j * 3 + 2] + syRow[j * 3 + 2] * syRow[j * 3 + 2]));
        }
    }

    return 0;
}
