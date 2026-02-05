#include "imageloadnode.h"

#include "flownodeautoregister.h"

ImageLoadNode::ImageLoadNode()
{
    _widget = new QWidget();
    auto lay = new QVBoxLayout(_widget);

    auto btn = new QPushButton("Open Image");
    _label = new QLabel("No file");

    lay->addWidget(btn);
    lay->addWidget(_label);

    connect(btn, &QPushButton::clicked, this, [this]()
    {
        QString file = QFileDialog::getOpenFileName(
            nullptr, "Open Image");

        if (!file.isEmpty())
        {
            _image.load(file);
            _label->setText(file);
        }
    });
}

std::shared_ptr<QtNodes::NodeData>
ImageLoadNode::outData(QtNodes::PortIndex)
{
    if (_image.isNull())
        return nullptr;

    return std::make_shared<ImageData>(_image);
}

void ImageLoadNode::compute()
{
    Q_EMIT dataUpdated(0);
}

REGISTER_NODE(ImageLoadNode, "Image")
