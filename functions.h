/*
 * Nick Cantalupa and Sean Duffy
 * 02/07/2025
 *
 * Functions for image matching
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Include necessary libraries
#include <iostream>
#include <opencv2/opencv.hpp>
#include "csv_util/csv_util.h"

// Function declarations
int get_src(const char* fp, cv::Mat& src);

// Function declarations
// Retrieve functions given cl arguments
int get_distance_function(const char* function_name,
                          std::function<float(const std::vector<float>&, const std::vector<float>&, float& distance)>&
                          distance_func);
int get_feature_function(const char* function_name, std::function<int(char*, std::vector<float>&)>& processing_func);

// Feature extraction
int features_7x7(char* fp, std::vector<float>& features);
int features_DNN(char* fp, std::vector<float>& features);
int histogram_hs(char* fp, std::vector<float>& features);
int histogram_hs_left(const cv::Mat& src, std::vector<float>& features);
int histogram_hs_right(const cv::Mat& src, std::vector<float>& features);
int histogram_hs_vertical_center(const cv::Mat& src, std::vector<float>& features);
int multihistogram_hs(char* fp, std::vector<float>& features);
int histogram_texture(const cv::Mat& src, std::vector<float>& features);
int texture_and_color(char* fp, std::vector<float>& features);
int features_faces(char* fp, std::vector<float>& features);
int bananas(char* fp, std::vector<float>& features);

// Distance metrics
int sum_squared_difference(const std::vector<float>& features1, const std::vector<float>& features2, float& distance);
int sum_absolute_difference(const std::vector<float>& features1, const std::vector<float>& features2, float& distance);
int histogram_intersection(const std::vector<float>& features1, const std::vector<float>& features2, float& distance);
int multihistogram_difference(const std::vector<float>& features1, const std::vector<float>& features2,
                              float& distance);
int texture_and_color_difference(const std::vector<float>& features1, const std::vector<float>& features2,
                                 float& distance);
int cosine_similarity(const std::vector<float>& features1, const std::vector<float>& features2, float& distance);
int banana_distance(const std::vector<float>& features1, const std::vector<float>& features2, float& distance);

// Image comparison
int closest_n_images(const std::vector<float>& features, const std::vector<std::vector<float>>& data,
                     const std::function<float(const std::vector<float>&, const std::vector<float>&, float& distance)>&
                     distance_function, const std::vector<char*>& filenames, int n, std::vector<char*>& topN);

// Image filters
int sobelX3x3(const cv::Mat &src, cv::Mat &dst );
int sobelY3x3(const cv::Mat &src, cv::Mat &dst );
int magnitude(const cv::Mat &sx, cv::Mat &sy, cv::Mat &dst );
#endif