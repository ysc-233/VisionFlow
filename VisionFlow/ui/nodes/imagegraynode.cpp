#include "imagegraynode.h"
#include "flownodeautoregister.h"

void ImageGrayNode::setInData(
    std::shared_ptr<QtNodes::NodeData> data,
    QtNodes::PortIndex)
{
    if (!FlowExecutionContext::running)
        return;

    auto img = std::dynamic_pointer_cast<ImageData>(data);
    if (!img)
        return;

    _out = img->image().convertToFormat(
        QImage::Format_Grayscale8);

    Q_EMIT dataUpdated(0);
}

std::shared_ptr<QtNodes::NodeData>
ImageGrayNode::outData(QtNodes::PortIndex)
{
    if (_out.isNull())
        return nullptr;

    return std::make_shared<ImageData>(_out);
}

REGISTER_NODE(ImageGrayNode, "Image")
