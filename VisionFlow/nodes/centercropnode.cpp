#include "centercropnode.h"
#include "flow/executor/flowexecutioncontext.h"
#include "nodes/flownodeautoregister.h"
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QVBoxLayout>
#include <QElapsedTimer>
QString CenterCropNode::staticNodeName = "ImageCenterCrop";
CenterCropNode::CenterCropNode()
{
    setStatus(FlowStatus::NodeStatus::Idle);
    _widget = new QWidget();
    auto layout = new QVBoxLayout(_widget);

    QLabel* label = new QLabel("Timeout (ms):");
    _timeoutSpin = new QSpinBox();
    _timeoutSpin->setRange(0, 60000);
    _timeoutSpin->setValue(5000);
    layout->addWidget(label);
    layout->addWidget(_timeoutSpin);
    timeoutMs = _timeoutSpin->value();
    QObject::connect(_timeoutSpin, qOverload<int>(&QSpinBox::valueChanged),
                     [this](int val){ timeoutMs = val; });
}

unsigned int CenterCropNode::nPorts(QtNodes::PortType type) const
{
    return type == QtNodes::PortType::In ? 2 : 1; // 输入: 图像 + int，输出: 图像
}

QtNodes::NodeDataType CenterCropNode::dataType(QtNodes::PortType type, QtNodes::PortIndex port) const
{
    if (type == QtNodes::PortType::In)
    {
        return port == 0 ? QtNodes::NodeDataType{"image","Image"}
                         : QtNodes::NodeDataType{"int","Integer"};
    }
    return {"image","Image"};
}

void CenterCropNode::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port)
{
    QMutexLocker locker(&_inputMutex);
    qDebug()<<__FUNCTION__;
    if (!FlowExecutionContext::running.load())
        return;

    if (port == 0) // 图像
    {
        auto img = std::dynamic_pointer_cast<ImageData>(data);
        if (img)
            _inputImg = img->matPtr();
    }
    else if (port == 1) // int
    {
        auto intData = std::dynamic_pointer_cast<IntNodeData>(data);
        if (intData)
            _inputInt = intData->ptr();
        qDebug()<<__FUNCTION__<<*_inputInt;
    }
}

std::shared_ptr<QtNodes::NodeData> CenterCropNode::outData(QtNodes::PortIndex)
{
    if (!_output || _output->empty()) return nullptr;
    return std::make_shared<ImageData>(_output);
}

QVariant CenterCropNode::getOutput(int)
{
    return QVariant::fromValue(_output);
}

void CenterCropNode::setInput(int port, const QVariant &data)
{
    QMutexLocker locker(&_inputMutex);
    if (port == 0) _inputImg = data.value<MatPtr>();
    else if (port == 1)
    {
        _inputInt = std::make_shared<int>(data.toInt());
        qDebug()<<__FUNCTION__<<*_inputInt;
    }
}

void CenterCropNode::compute()
{
    if (!_inputImg || _inputImg->empty() || !_inputInt)
        return;

    int inputNum;
    {
        QMutexLocker locker(&_inputMutex);
        inputNum = *_inputInt;
    }

    if (inputNum != 1)
    {
        if (getStatus() == FlowStatus::Idle)
        {
            waitTimer.start();
            setStatus(FlowStatus::Waiting);
        }
        else if (getStatus() == FlowStatus::Waiting)
        {
            qint64 elapsed = waitTimer.elapsed();
            if (timeoutMs > 0 && elapsed >= timeoutMs)
            {
                qDebug() << "[CenterCropNode] Timeout";
                setStatus(FlowStatus::Timeout);
                FlowExecutionContext::running.store(false);
            }
        }
        return;
    }

    setStatus(FlowStatus::Running);

    cv::Mat src = *_inputImg;
    int w = src.cols / 2;
    int h = src.rows / 2;
    int x = src.cols / 4;
    int y = src.rows / 4;

    if (!_output)
        _output = std::make_shared<cv::Mat>();

    *_output = src(cv::Rect(x, y, w, h)).clone();
    qDebug() << "[CenterCropNode] Center crop done";

    setStatus(FlowStatus::Done);
}

REGISTER_NODE(CenterCropNode, "Image")
