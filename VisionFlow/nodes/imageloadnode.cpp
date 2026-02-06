#include "imageloadnode.h"
#include "utils/imageconvert.h"
#include "flownodeautoregister.h"
#include "QMessageBox"
#include "flow/flowtypes.h"

ImageLoadNode::ImageLoadNode()
{
    flowNodeName = "ImageLoad";
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
            QImage img;
            if (!img.load(file))
                return;

            auto imgcv = QImageToMat(img);
            _image = std::make_shared<cv::Mat>(std::move(imgcv));
            _label->setText(file);
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
}

REGISTER_NODE(ImageLoadNode, "Image")
