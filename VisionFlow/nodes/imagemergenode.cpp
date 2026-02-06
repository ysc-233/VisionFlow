#include "imagemergenode.h"
#include "flownodeautoregister.h"
#include <opencv2/opencv.hpp>
#include "flow/flowtypes.h"
ImageMergeNode::ImageMergeNode()
{
    flowNodeName = "ImageMerge";
    _widget = new QWidget();
    auto lay = new QVBoxLayout(_widget);

    _modeBox = new QComboBox();
    _modeBox->addItem("Horizontal");
    _modeBox->addItem("Vertical");

    lay->addWidget(_modeBox);
}

void ImageMergeNode::setInData(
    std::shared_ptr<QtNodes::NodeData> data,
    QtNodes::PortIndex port)
{
    if (!FlowExecutionContext::running.load())
        return;

    auto img = std::dynamic_pointer_cast<ImageData>(data);
    if (!img)
        return;

    _inputs[port] = img->matPtr();
    _ready[port] = true;
}

std::shared_ptr<QtNodes::NodeData>
ImageMergeNode::outData(QtNodes::PortIndex)
{
    if (!_out || _out->empty())
        return nullptr;

    return std::make_shared<ImageData>(_out);
}

void ImageMergeNode::setInput(int port, const QVariant &data)
{
    auto mat = data.value<MatPtr>();;

    if (!mat || mat->empty())
        return;

    _inputs[port] = mat;
    _ready[port] = true;
}

QVariant ImageMergeNode::getOutput(int port)
{
    if (!_out || _out->empty())
        return {};

    return QVariant::fromValue(_out);
}
void ImageMergeNode::compute()
{
    if (!_ready[0] || !_ready[1])
    {
        qDebug()<<__FUNCTION__<< "input empty!";
        return;
    }
    auto& aPtr = _inputs[0];
    auto& bPtr = _inputs[1];

    if (!aPtr || !bPtr ||
        aPtr->empty() || bPtr->empty())
        return;

    const cv::Mat& a = *aPtr;
    const cv::Mat& b = *bPtr;

    bool horizontal =
        (_modeBox->currentIndex() == 0);

    cv::Mat a2, b2;

    // ---------- 通道统一 ----------
    if (a.channels() != b.channels())
    {
        if (a.channels() == 1)
            cv::cvtColor(a, a2, cv::COLOR_GRAY2BGR);
        else
            a2 = a;

        if (b.channels() == 1)
            cv::cvtColor(b, b2, cv::COLOR_GRAY2BGR);
        else
            b2 = b;
    }
    else
    {
        a2 = a;
        b2 = b;
    }

    // ---------- 尺寸对齐 ----------
    cv::Mat ra, rb;

    if (horizontal)
    {
        int height = std::max(a2.rows, b2.rows);

        if (a2.rows != height)
            cv::resize(a2, ra,
                       cv::Size(a2.cols, height));
        else
            ra = a2;

        if (b2.rows != height)
            cv::resize(b2, rb,
                       cv::Size(b2.cols, height));
        else
            rb = b2;

        _out = std::make_shared<cv::Mat>();
        cv::hconcat(ra, rb, *_out);
    }
    else
    {
        int width = std::max(a2.cols, b2.cols);

        if (a2.cols != width)
            cv::resize(a2, ra,
                       cv::Size(width, a2.rows));
        else
            ra = a2;

        if (b2.cols != width)
            cv::resize(b2, rb,
                       cv::Size(width, b2.rows));
        else
            rb = b2;

        _out = std::make_shared<cv::Mat>();
        cv::vconcat(ra, rb, *_out);
    }
    _ready[0] = false;
    _ready[1] = false;
}


REGISTER_NODE(ImageMergeNode, "Image")
