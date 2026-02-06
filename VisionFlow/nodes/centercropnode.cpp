#include "centercropnode.h"
#include "flow/executor/flowexecutioncontext.h"
#include "nodes/flownodeautoregister.h"
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QVBoxLayout>
#include <QElapsedTimer>
CenterCropNode::CenterCropNode()
{
    flowNodeName = "CenterCrop";

    _widget = new QWidget();
    auto layout = new QVBoxLayout(_widget);

    QLabel* label = new QLabel("Timeout (ms):");
    _timeoutSpin = new QSpinBox();
    _timeoutSpin->setRange(0, 60000);
    _timeoutSpin->setValue(5000); // 默认 5 秒
    layout->addWidget(label);
    layout->addWidget(_timeoutSpin);

    // 当 UI 修改时更新节点内部变量
    connect(_timeoutSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
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
    if (port == 0) _inputImg = data.value<MatPtr>();
    else if (port == 1) _inputInt = std::make_shared<int>(data.toInt());
}

void CenterCropNode::compute()
{
    if (!_inputImg || _inputImg->empty() || !_inputInt)
        return;

    QElapsedTimer timer;
    timer.start();

    // 等待 int 为 1 或超时
    while (*_inputInt != 1 && FlowExecutionContext::running.load())
    {
        QThread::msleep(10); // 防止 CPU 空循环
        if (timeoutMs > 0 && timer.elapsed() >= timeoutMs)
        {
            qDebug() << "[CenterCropNode] Timeout reached, stopping flow!";
            FlowExecutionContext::running.store(false); // 停止流程
            return;
        }
    }

    if (!FlowExecutionContext::running.load())
        return;

    cv::Mat src = *_inputImg;
    int w = src.cols / 2;
    int h = src.rows / 2;
    int x = src.cols / 4;
    int y = src.rows / 4;

    if (!_output)
        _output = std::make_shared<cv::Mat>();

    *_output = src(cv::Rect(x, y, w, h)).clone();
    qDebug() << "[CenterCropNode] Center crop done";
}
REGISTER_NODE(CenterCropNode, "CenterCropNode")
