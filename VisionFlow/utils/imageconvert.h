#ifndef IMAGECONVERT_H
#define IMAGECONVERT_H
#pragma once

#include <QImage>
#include <opencv2/opencv.hpp>

cv::Mat QImageToMat(const QImage& image);
QImage MatToQImage(const cv::Mat& mat);
#endif // IMAGECONVERT_H
