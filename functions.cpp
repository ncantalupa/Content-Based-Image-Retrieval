/*
 * Nick Cantalupa and Sean Duffy
 * 02/07/2025
 *
 * Functions for image matching
 *
 * Distance metrics (command line arg: function name)
 * ssd: sum_squared_difference
 * hist_intersection: histogram_intersection
 * multihistogram_diff: multihistogram_difference
 * texture_color_diff: texture_and_color_difference
 * cosine: cosine_similarity
 * banana_distance: banana_distance
 *
 * Processing functions (command line arg: function name)
 * baseline: features_7x7
 * histogram: histogram_hs
 * multihistogram: multihistogram_hs
 * texture_color: texture_and_color
 * ResNet: features_DNN
 * cans: features_can
 * faces: features_faces
 * bananas: bananas
 *
 */


#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"
#include "functions.h"
#include "DA2Network.hpp"
#include "faceDetect.h"

int HIST_SIZE = 16; // Don't set too high, and ensure it's used in all histogram functions and distance metrics

// Extract center 7x7 pixels from image as features
int features_7x7(char* fp, std::vector<float>& features)
{
    // Retrieve image
    cv::Mat src;
    get_src(fp, src);

    // Find center of the image
    const int rows_center = src.rows / 2;
    const int cols_center = src.cols / 2;
    const int rows_start = rows_center - 3;
    const int cols_start = cols_center - 3;
    const int rows_end = rows_center + 3;
    const int cols_end = cols_center + 3;

    // Add features from center kernel
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

// Extract ResNet embeddings from file as features
int features_DNN(char* fp, std::vector<float>& features)
{
    // Read in ResNet embdeddings
    char* resnet_features = new char[strlen("ResNet18_olym.csv") + 1];
    strcpy(resnet_features, "../features/ResNet18_olym.csv");
    std::vector<char*> filenames;
    std::vector<std::vector<float>> data;
    read_image_data_csv(resnet_features, filenames, data, 0);

    // Get features from target row
    for (int i = 0; i < filenames.size(); i++)
    {
        if (strcmp(filenames[i], fp) == 0)
        {
            for (int j = 0; j < data[i].size(); j++)
            {
                features.push_back(data[i][j]);
            }
            break;
        }
    }
    return 0;
}

// Isolate can and extract histrogam features
int features_can(char* fp, std::vector<float>& features)
{
    cv::Mat src;
    get_src(fp, src);
    cv::Mat depth = cv::Mat::zeros(src.size(), CV_8UC3);
    cv::Mat dst = cv::Mat::zeros(src.size(), CV_8UC3);

    // Run DA network on can image
    DA2Network da_net("model_fp16.onnx");
    da_net.set_input(src);
    da_net.run_network(depth, src.size());

    // Get center of the image
    int center_i = src.rows / 2;
    int center_j = src.cols / 2;
    int r = 50;

    // Find the average pixel values of the center of the image
    int B_avg = 0;
    int G_avg = 0;
    int R_avg = 0;
    for (int i = center_i - r; i < center_i + r; i++)
    {
        for (int j = center_j - r; j < center_j + r; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                switch (k)
                {
                case 0:
                    B_avg += src.at<cv::Vec3b>(i, j)[k];
                    break;
                case 1:
                    G_avg += src.at<cv::Vec3b>(i, j)[k];
                    break;
                case 2:
                    R_avg += src.at<cv::Vec3b>(i, j)[k];
                    break;
                default:
                    break;
                }
            }
        }
    }
    B_avg = B_avg / ((r * 2 + 1) * (r * 2 + 1));
    G_avg = G_avg / ((r * 2 + 1) * (r * 2 + 1));
    R_avg = R_avg / ((r * 2 + 1) * (r * 2 + 1));

    // Isolate the foreground pixels, replace the background with avg color calculated above
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            if (depth.at<unsigned char>(i, j) > 120)
            {
                dst.at<cv::Vec3b>(i, j) = src.at<cv::Vec3b>(i, j);
            }
            else
            {
                dst.at<cv::Vec3b>(i, j) = cv::Vec3b(B_avg, G_avg, R_avg);
            }
        }
    }

    // Feed new image into Histogram function
    char* out_fp = new char[strlen("DA2.jpg") + 1];
    strcpy(out_fp, "DA2.jpg");
    cv::imwrite(out_fp, dst);
    histogram_hs(out_fp, features);
    return 0;
}

// Calculate hue saturation histogram features
int histogram_hs(char* fp, std::vector<float>& features)
{
    cv::Mat src;
    get_src(fp, src);

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

// Same histogram_hs for left half of image
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

// Same as histogram_hs for center half of image
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

// Same as histogram_hs for right half of image
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

// Combines left, right, and center histograms into one feature vector
int multihistogram_hs(char* fp, std::vector<float>& features)
{
    cv::Mat src;
    get_src(fp, src);

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

// Calculated gradients magnitudes and creates histogram
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


// Combines texture and color histograms into one feature vector
int texture_and_color(char* fp, std::vector<float>& features)
{
    cv::Mat src;
    get_src(fp, src);

    // Calculate histograms
    std::vector<float> texture_features;
    std::vector<float> color_features;
    histogram_texture(src, texture_features);
    histogram_hs(fp, color_features);

    // Append histogram features to the output features vector
    features.insert(features.end(), texture_features.begin(), texture_features.end());
    features.insert(features.end(), color_features.begin(), color_features.end());

    return 0;
}

// Extracts number and size of faces in the image, creates feature vector
int features_faces(char* fp, std::vector<float>& features)
{
    // Get image
    cv::Mat src;
    get_src(fp, src);

    // Identify faces in image
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    std::vector<cv::Rect> faces;
    detectFaces(gray, faces);
    drawBoxes(src, faces, 30, 1.0);
    features.clear();
    // Get number of faces
    int MAX_FACES = 5;
    features.push_back((std::min((int)faces.size(), MAX_FACES)) * 20);

    // Add Face box size to featrues for each face
    for (size_t i = 0; i < MAX_FACES; i++)
    {
        if (i < faces.size())
        {
            features.push_back(faces[i].width);
            features.push_back(faces[i].height);
        }
        else
        {
            features.push_back(0);
            features.push_back(0);
        }
    }
    return 0;
}

// Extracs yellow spatial variance, yellow percentage, and yellow density near center of yellow mass
int bananas(char* fp, std::vector<float>& features)
{
    // Initialize images
    cv::Mat src;
    get_src(fp, src);
    cv::Mat original = src.clone(); // duplicate for visualization during debugging

    // Convert image to HSV
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    // Calculate histogram for HS components
    std::vector<cv::Point> yellow_pixels;
    for (int i = 0; i < hsv.rows; i++)
    {
        auto* row = hsv.ptr<cv::Vec3b>(i);
        for (int j = 0; j < hsv.cols; j++)
        {
            // Set HSV thresholds for yellow
            constexpr int s_lower = 250;
            constexpr int v_lower = 180;
            constexpr int yellow_hue_higher = 35;
            constexpr int yellow_hue_lower = 25;

            // Get HSV values
            const int h = row[j][0];
            const int s = row[j][1];
            const int v = row[j][2];

            // Check if pixel is yellow and add to vector
            if (yellow_hue_lower < h && h < yellow_hue_higher && s > s_lower && v > v_lower)
            {
                yellow_pixels.emplace_back(j, i);
            }

            else
            {
                // Setting non-yellow pixels to black (use for debugging: set breakpoint after for loops
                src.at<cv::Vec3b>(i, j)[0] = 0;
                src.at<cv::Vec3b>(i, j)[1] = 0;
                src.at<cv::Vec3b>(i, j)[2] = 0;
            }
        }
    }

    // Calculations on yellow pixels
    if (!yellow_pixels.empty())
    {
        // Get standard deviation of yellow pixels in x and y directions and add to feature vector
        cv::Scalar mean, stddev;
        cv::meanStdDev(yellow_pixels, mean, stddev);
        const float normalized_stddev_x = static_cast<float>(stddev[0]) / hsv.rows;
        const float normalized_stddev_y = static_cast<float>(stddev[1]) / hsv.cols;
        features.push_back(normalized_stddev_x);
        features.push_back(normalized_stddev_y);
        // Calculate percentage of yellow pixels and add to feature vector
        float yellow_percentage = static_cast<float>(yellow_pixels.size()) / (hsv.rows * hsv.cols);
        features.push_back(yellow_percentage);

        // Calculate density of yellow pixels near the center
        int x_limit = hsv.cols / 4;
        int y_limit = hsv.rows / 4;
        int count_near_center = 0;
        // Iterate through yellow pixels and increment count if within x and y limits of center
        for (const auto& point : yellow_pixels)
        {
            if (std::abs(point.x - mean[0]) < x_limit && std::abs(point.y - mean[1]) < y_limit)
            {
                count_near_center++;
            }
        }
        // Calculate density and add to feature vector
        float density_near_center = static_cast<float>(count_near_center) / (x_limit * y_limit);
        features.push_back(density_near_center);
    }
    else
    {
        // Default values to add if no yellow pixels are present
        features.push_back(1);
        features.push_back(1);
        features.push_back(0);
        features.push_back(0);
    }

    return 0;
}

// Get the closest n images for the given distance function and target file
int closest_n_images(const std::vector<float>& features, const std::vector<std::vector<float>>& data,
                     const std::function<float(const std::vector<float>&, const std::vector<float>&, float& distance)>&
                     distance_function,
                     const std::vector<char*>& filenames, const int n, std::vector<char*>& topN)
{
    // Get distance function
    std::vector<float> ssd;
    for (const auto& i : data)
    {
        float distance;
        distance_function(features, i, distance);
        ssd.push_back(distance);
    }
    // Find nearest images, add them to vector
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

// Get sum of squared differences between two feature vectors
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

// Get sum of absolute differences between two feature vectors
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

// Get cosine similarity between two feature vectors
int cosine_similarity(const std::vector<float>& features1, const std::vector<float>& features2, float& distance)
{
    float dot_product = 0;
    float norm1 = 0;
    float norm2 = 0;
    for (int i = 0; i < features1.size(); i++)
    {
        dot_product += features1[i] * features2[i];
        norm1 += features1[i] * features1[i];
        norm2 += features2[i] * features2[i];
    }
    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);
    distance = 1 - dot_product / (norm1 * norm2);
    return 0;
}

// Get histogram intersection between two feature vectors
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

// Get multihistogram difference between two feature vectors
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

    // Calculate sum absolute difference distances between the three histograms
    float left_distance;
    float right_distance;
    float center_distance;
    sum_absolute_difference(left1, left2, left_distance);
    sum_absolute_difference(right1, right2, right_distance);
    sum_absolute_difference(center1, center2, center_distance);

    // Combine distance metrics with weightings
    distance = 0.15f * left_distance + 0.15f * right_distance + 0.7f * center_distance;

    return 0;
}

// Get texture and color difference between two feature vectors
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

    // Combine distance metrics with weightings
    distance = 0.5f * texture_distance + 0.5f * color_distance;

    return 0;
}

// Get banana distance between two feature vectors (SSD of spatial variance, percent yellow, and yellow density)
int banana_distance(const std::vector<float>& features1, const std::vector<float>& features2, float& distance)
{
    // Calculate SSD for the spatial variances
    float ssd_spatial_variance = (features1[0] - features2[0]) * (features1[0] - features2[0]) +
        (features1[1] - features2[1]) * (features1[1] - features2[1]);

    // Calculate SSD for the percent yellow
    float ssd_percent_yellow = (features1[2] - features2[2]) * (features1[2] - features2[2]);

    // Calculate SSD for the yellow density
    float ssd_yellow_density = (features1[3] - features2[3]) * (features1[3] - features2[3]);

    // Combine the SSDs with weighting
    distance = 0.50f * ssd_spatial_variance; //+ 0.25f * ssd_yellow_density + 0.25f * ssd_percent_yellow;

    return 0;
}

// Given a command line argument return the corresponding image featurization function
int get_feature_function(const char* function_name, std::function<int(char*, std::vector<float>&)>& processing_func)
{
    std::map<std::string, std::function<int(char*, std::vector<float>&)>> processing_functions = {
        // Add processing functions here
        {"baseline", features_7x7},
        {"histogram", histogram_hs},
        {"multihistogram", multihistogram_hs},
        {"texture_color", texture_and_color},
        {"ResNet", features_DNN},
        {"cans", features_can},
        {"faces", features_faces},
        {"bananas", bananas}
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

// Given a command line argument return the corresponding distance metric function
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
            {"texture_color_diff", texture_and_color_difference},
            {"cosine", cosine_similarity},
            {"banana_distance", banana_distance}
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

// Gets image from file path
int get_src(const char* fp, cv::Mat& src)
{
    src = cv::imread(fp);
    if (src.empty())
    {
        std::cerr << "Unable to open image: " << fp << std::endl;
        return -1;
    }
    return 0;
}
