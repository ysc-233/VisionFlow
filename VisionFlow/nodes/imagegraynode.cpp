#include "imagegraynode.h"
#include "flownodeautoregister.h"
QString ImageGrayNode::staticNodeName = "ImageGray";
void ImageGrayNode::setInData(
    std::shared_ptr<QtNodes::NodeData> data,
    QtNodes::PortIndex)
{
    if (!FlowExecutionContext::running.load())
        return;

    auto img =
        std::dynamic_pointer_cast<ImageData>(data);

    if (!img)
        return;

    auto input = img->matPtr();

    if (!input || input->empty())
        return;
    _input = input;
}

std::shared_ptr<QtNodes::NodeData>
ImageGrayNode::outData(QtNodes::PortIndex)
{
    if (!_out || _out->empty())
        return nullptr;

    return std::make_shared<ImageData>(_out);
}

QVariant ImageGrayNode::getOutput(int port)
{
    return QVariant::fromValue(_out);
}

void ImageGrayNode::setInput(int port, const QVariant &data)
{
    if (!data.isValid())
    {
        _input.reset();
        return;
    }

    _input = data.value<MatPtr>();
}

void ImageGrayNode::compute()
{
    if (!_input || _input->empty())
        return;

    _out = std::make_shared<cv::Mat>();

    if (_input->channels() == 3)
    {
        cv::cvtColor(*_input, *_out,
                     cv::COLOR_BGR2GRAY);
    }
    else
    {
        *_out = _input->clone();
    }
    setStatus(FlowStatus::NodeStatus::Done);
}

REGISTER_NODE(ImageGrayNode, "Image")
