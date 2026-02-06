#include "imageconvert.h"

cv::Mat QImageToMat(const QImage& image)
{
    QImage img = image.convertToFormat(QImage::Format_RGB888);

    cv::Mat mat(
        img.height(),
        img.width(),
        CV_8UC3,
        (void*)img.bits(),
        img.bytesPerLine());

    cv::Mat result;
    cv::cvtColor(mat, result, cv::COLOR_RGB2BGR);

    return result.clone(); // 必须 clone
}

QImage MatToQImage(const cv::Mat& mat)
{
    if (mat.empty())
        return QImage();

    cv::Mat rgb;

    if (mat.channels() == 3)
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    else if (mat.channels() == 1)
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
    else
        return QImage();

    return QImage(
        rgb.data,
        rgb.cols,
        rgb.rows,
        rgb.step,
        QImage::Format_RGB888).copy();
}
