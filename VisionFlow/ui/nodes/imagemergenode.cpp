#include "imagemergenode.h"
#include "flownodeautoregister.h"
#include <QPainter>
ImageMergeNode::ImageMergeNode()
{
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
    if (!FlowExecutionContext::running)
        return;

    auto img = std::dynamic_pointer_cast<ImageData>(data);
    if (!img)
        return;

    _inputs[port] = img->image();
    _ready[port] = true;

    if (_ready[0] && _ready[1])
        compute();
}

void ImageMergeNode::compute()
{
    auto a = _inputs[0];
    auto b = _inputs[1];

    if (a.isNull() || b.isNull())
        return;

    bool horizontal =
        (_modeBox->currentIndex() == 0);

    if (horizontal)
    {
        int width = a.width() + b.width();
        int height = std::max(a.height(), b.height());

        _out = QImage(width, height,
                      QImage::Format_RGB32);
        _out.fill(Qt::black);

        QPainter p(&_out);
        p.drawImage(0, 0, a);
        p.drawImage(a.width(), 0, b);
        p.end();
    }
    else
    {
        int width = std::max(a.width(), b.width());
        int height = a.height() + b.height();

        _out = QImage(width, height,
                      QImage::Format_RGB32);
        _out.fill(Qt::black);

        QPainter p(&_out);
        p.drawImage(0, 0, a);
        p.drawImage(0, a.height(), b);
        p.end();
    }

    Q_EMIT dataUpdated(0);

    _ready[0] = false;
    _ready[1] = false;
}

std::shared_ptr<QtNodes::NodeData>
ImageMergeNode::outData(QtNodes::PortIndex)
{
    if (_out.isNull())
        return nullptr;

    return std::make_shared<ImageData>(_out);
}

REGISTER_NODE(ImageMergeNode, "Image")
