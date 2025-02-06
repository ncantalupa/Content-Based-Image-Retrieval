/*************************************
 * Nick Cantalupa
 * Jan 13 2025
 * CS 5330 Computer Vision
 * Project 1.2: Header file that contains the filter functions for displaying live video feed
 *************************************/
#ifndef FILTERS_H
#define FILTERS_H

int grayscale(cv::Mat &src, cv::Mat &dst);
int sepia(cv::Mat &src, cv::Mat &dst);
int blur5x5_1(cv::Mat &src, cv::Mat &dst);
int blur5x5_2(cv::Mat &src, cv::Mat &dst);
int sobelX3x3(cv::Mat &src, cv::Mat &dst );
int sobelY3x3(cv::Mat &src, cv::Mat &dst );
int magnitude( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst );
int blurQuantize( cv::Mat &src, cv::Mat &dst, int levels );
int colorSubject(cv::Mat &src, cv::Mat &temp, cv::Mat &dst);
int negative(cv::Mat &src, cv::Mat &dst);
int brightness(cv::Mat &src, cv::Mat &dst, int value);
int medianBlur(cv::Mat &src, cv::Mat &dst);
int photoBackground(cv::Mat &src, cv::Mat &temp, cv::Mat &dst);
int colorful(cv::Mat &src, cv::Mat &dst);
int closestColor(int &b, int &g, int &r);

#endif