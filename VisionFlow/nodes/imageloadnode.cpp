#include "imageloadnode.h"
#include "utils/imageconvert.h"
#include "flownodeautoregister.h"
#include "QMessageBox"
#include "flow/flowtypes.h"
QString ImageLoadNode::staticNodeName = "ImageLoad";
ImageLoadNode::ImageLoadNode()
{
    setStatus(FlowStatus::NodeStatus::Idle);
    _widget = new QWidget();
    auto lay = new QVBoxLayout(_widget);

    auto btn = new QPushButton("Open Image");
    _label = new QLabel("No file");

    lay->addWidget(btn);
    lay->addWidget(_label);

    connect(btn, &QPushButton::clicked, this, [this]()
    {
        _filePath = QFileDialog::getOpenFileName(
            nullptr, "Open Image");

        if (!_filePath.isEmpty())
        {
            QImage img;
            if (!img.load(_filePath))
                return;

            auto imgcv = QImageToMat(img);
            _image = std::make_shared<cv::Mat>(std::move(imgcv));
            _label->setText(_filePath);
        }
    });
}

std::shared_ptr<QtNodes::NodeData>
ImageLoadNode::outData(QtNodes::PortIndex)
{
    if (!_image || _image->empty())
        return nullptr;

    return std::make_shared<ImageData>(_image);
}

QVariant ImageLoadNode::getOutput(int port)
{
    return QVariant::fromValue(_image);
}

void ImageLoadNode::compute()
{
    if (!_image || _image->empty())
        qDebug() << "ImageLoad: empty image";
    setStatus(FlowStatus::NodeStatus::Done);
    Q_EMIT dataUpdated(0);
}

QJsonObject ImageLoadNode::saveData() const
{
    qDebug()<<__FUNCTION__;
    QJsonObject obj;
    if (!_filePath.isEmpty())
        obj["filePath"] = _filePath;
    return obj;
}

void ImageLoadNode::loadData(const QJsonObject &obj)
{
    _filePath = obj["filePath"].toString();

    if (_filePath.isEmpty())
    {
        _label->setText("No file");
        _image.reset();
        return;
    }

    QImage img;
    if (!img.load(_filePath))
    {
        _label->setText("Load failed");
        _image.reset();
        return;
    }

    auto imgcv = QImageToMat(img);
    _image = std::make_shared<cv::Mat>(std::move(imgcv));

    _label->setText(_filePath);
}

REGISTER_NODE(ImageLoadNode, "Image")
