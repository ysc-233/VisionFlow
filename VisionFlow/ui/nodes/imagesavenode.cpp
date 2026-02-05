#include "imagesavenode.h"
#include "flownodeautoregister.h"

ImageSaveNode::ImageSaveNode()
{
    _widget = new QWidget();
    auto lay = new QVBoxLayout(_widget);

    auto btn = new QPushButton("Save Path");
    _label = new QLabel("No path");

    lay->addWidget(btn);
    lay->addWidget(_label);

    connect(btn, &QPushButton::clicked, this, [this]()
    {
        _path = QFileDialog::getSaveFileName(
            nullptr, "Save Image");

        _label->setText(_path);
    });
}

void ImageSaveNode::setInData(
    std::shared_ptr<QtNodes::NodeData> data,
    QtNodes::PortIndex)
{
    if (!FlowExecutionContext::running)
        return;

    auto img = std::dynamic_pointer_cast<ImageData>(data);
    if (!img || _path.isEmpty())
        return;

    img->image().save(_path);
}

REGISTER_NODE(ImageSaveNode, "Image")
