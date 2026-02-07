#include "imagesavenode.h"
#include "flownodeautoregister.h"
#include "flow/flowtypes.h"
QString ImageSaveNode::staticNodeName = "ImageSave";
ImageSaveNode::ImageSaveNode()
{
    setStatus(FlowStatus::NodeStatus::Idle);
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
    if (!FlowExecutionContext::running.load())
        return;

    auto img =
        std::dynamic_pointer_cast<ImageData>(data);

    if (!img || _path.isEmpty())
        return;

    auto matPtr = img->matPtr();

    if (!matPtr || matPtr->empty())
        return;
    _input = matPtr;
}

void ImageSaveNode::setInput(int port, const QVariant &data)
{
     _input = data.value<MatPtr>();
}

void ImageSaveNode::compute()
{
    if (!_input || _path.isEmpty())
        return;

    cv::imwrite(_path.toStdString(), *_input);
    setStatus(FlowStatus::NodeStatus::Done);
}

REGISTER_NODE(ImageSaveNode, "Image")
