#include "templatmatchnode.h"
#include "flownodeautoregister.h"
#include "utils/imageconvert.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>

QString TemplateMatchNode::staticNodeName = "TemplateMatch";

// ============================================================================
// TemplateSelectionView 实现
// ============================================================================
TemplateSelectionView::TemplateSelectionView(QWidget* parent)
    : QGraphicsView(parent), _scene(nullptr)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setDragMode(QGraphicsView::NoDrag);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

TemplateSelectionView::~TemplateSelectionView()
{
    if (_scene) {
        delete _scene;
        _scene = nullptr;
    }
}

void TemplateSelectionView::setImage(const cv::Mat& img)
{
    _image = img.clone();
    if (_image.empty())
        return;

    QImage qimg = MatToQImage(_image);
    QPixmap pixmap = QPixmap::fromImage(qimg);

    if (_scene) {
        delete _scene;  // 会自动删除所有 items
    }
    _selectionRect = nullptr;

    _scene = new QGraphicsScene(this);
    _scene->addPixmap(pixmap);
    setScene(_scene);
    fitInView(_scene->sceneRect(), Qt::KeepAspectRatio);
}

cv::Rect TemplateSelectionView::getSelectionRect() const
{
    if (!_selectionRect || _image.empty())
        return cv::Rect();

    QRectF rectF = _selectionRect->rect();
    QPointF topLeft = rectF.topLeft();
    QPointF bottomRight = rectF.bottomRight();

    QRectF sceneRect = _scene->sceneRect();
    if (sceneRect.isEmpty())
        return cv::Rect();

    double scaleX = _image.cols / sceneRect.width();
    double scaleY = _image.rows / sceneRect.height();

    int x = std::max(0, (int)(topLeft.x() * scaleX));
    int y = std::max(0, (int)(topLeft.y() * scaleY));
    int w = std::max(1, (int)((bottomRight.x() - topLeft.x()) * scaleX));
    int h = std::max(1, (int)((bottomRight.y() - topLeft.y()) * scaleY));

    x = std::min(x, _image.cols - 1);
    y = std::min(y, _image.rows - 1);
    w = std::min(w, _image.cols - x);
    h = std::min(h, _image.rows - y);

    return cv::Rect(x, y, w, h);
}

void TemplateSelectionView::clearSelection()
{
    if (_selectionRect) {
        delete _selectionRect;
        _selectionRect = nullptr;
    }
    _isSelecting = false;
}

void TemplateSelectionView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _startPos = mapToScene(event->pos());
        _isSelecting = true;

        if (_selectionRect) {
            if (_scene) {
                _scene->removeItem(_selectionRect);
            }
            delete _selectionRect;
            _selectionRect = nullptr;
        }

        _selectionRect = new QGraphicsRectItem();
        _selectionRect->setPen(QPen(Qt::red, 2, Qt::DashLine));
        _selectionRect->setBrush(QColor(255, 0, 0, 50));

        if (_scene) {
            _scene->addItem(_selectionRect);
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void TemplateSelectionView::mouseMoveEvent(QMouseEvent* event)
{
    if (_isSelecting && _selectionRect) {
        _endPos = mapToScene(event->pos());
        QRectF rect(_startPos, _endPos);
        rect = rect.normalized();
        _selectionRect->setRect(rect);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void TemplateSelectionView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _isSelecting = false;
        emit selectionReleased();
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// ============================================================================
// TemplateMatchDialog 实现
// ============================================================================
TemplateMatchDialog::TemplateMatchDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Create Template");
    setMinimumSize(800, 600);
    setWindowModality(Qt::ApplicationModal);

    auto layout = new QVBoxLayout(this);

    _view = new TemplateSelectionView(this);
    layout->addWidget(_view);

    auto btnLayout = new QHBoxLayout();
    _btnLoad = new QPushButton("Load Image");
    _btnSave = new QPushButton("Save Template");
    _btnCancel = new QPushButton("Close");
    _btnSave->setEnabled(false);

    btnLayout->addWidget(_btnLoad);
    btnLayout->addWidget(_btnSave);
    btnLayout->addWidget(_btnCancel);
    layout->addLayout(btnLayout);

    connect(_btnLoad, &QPushButton::clicked, this, &TemplateMatchDialog::onLoadImageClicked);
    connect(_btnSave, &QPushButton::clicked, this, &TemplateMatchDialog::onSaveClicked);
    connect(_btnCancel, &QPushButton::clicked, this, &TemplateMatchDialog::onCancelClicked);
    connect(_view, &TemplateSelectionView::selectionReleased, this, [this]() {
        cv::Rect rect = _view->getSelectionRect();
        if (rect.area() > 0) {
            _btnSave->setEnabled(true);
        }
    });
}

TemplateMatchDialog::~TemplateMatchDialog()
{
}

void TemplateMatchDialog::setImage(const cv::Mat& img)
{
    _sourceImage = img.clone();
    if (!_sourceImage.empty()) {
        _view->setImage(_sourceImage);
    }
    _hasSelection = false;
    _btnSave->setEnabled(false);
}

cv::Mat TemplateMatchDialog::getTemplateImage() const
{
    return _templateImage.clone();
}

cv::Rect TemplateMatchDialog::getTemplateRect() const
{
    return _templateRect;
}

bool TemplateMatchDialog::hasSelection() const
{
    return _hasSelection;
}

void TemplateMatchDialog::onLoadImageClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Load Template Image", "", "Images (*.png *.jpg *.bmp *.tif)");

    if (filePath.isEmpty())
        return;

    QImage img;
    if (!img.load(filePath)) {
        QMessageBox::critical(this, "Error", "Failed to load image!");
        return;
    }

    cv::Mat mat = QImageToMat(img);
    _sourceImage = mat;
    _view->setImage(_sourceImage);
    _hasSelection = false;
    _btnSave->setEnabled(false);
}

void TemplateMatchDialog::onSaveClicked()
{
    cv::Rect rect = _view->getSelectionRect();
    if (rect.area() <= 0) {
        QMessageBox::warning(this, "Warning", "Please select a template region!");
        return;
    }

    if (_sourceImage.empty()) {
        QMessageBox::warning(this, "Warning", "No image loaded!");
        return;
    }

    _templateRect = rect;
    _templateImage = _sourceImage(_templateRect).clone();
    _hasSelection = true;

    QMessageBox::information(this, "Success",
        QString("Template saved!\nSize: %1 x %2").arg(_templateRect.width).arg(_templateRect.height));
}

void TemplateMatchDialog::onCancelClicked()
{
    close();
}

// ============================================================================
// TemplateMatchNode 实现
// ============================================================================
TemplateMatchNode::TemplateMatchNode()
{
    setStatus(FlowStatus::NodeStatus::Idle);

    _widget = new QWidget();
    auto layout = new QVBoxLayout(_widget);

    _btnCreateTemplate = new QPushButton("Create Template");
    layout->addWidget(_btnCreateTemplate);

    auto thresholdLayout = new QHBoxLayout();
    thresholdLayout->addWidget(new QLabel("Threshold:"));
    _thresholdSpin = new QDoubleSpinBox();
    _thresholdSpin->setRange(0.0, 1.0);
    _thresholdSpin->setSingleStep(0.05);
    _thresholdSpin->setValue(_matchThreshold);
    thresholdLayout->addWidget(_thresholdSpin);
    layout->addLayout(thresholdLayout);

    _templateInfoLabel = new QLabel("No template");
    _templateInfoLabel->setStyleSheet("color: gray;");
    layout->addWidget(_templateInfoLabel);

    connect(_btnCreateTemplate, &QPushButton::clicked, this, &TemplateMatchNode::onCreateTemplateClicked);
    connect(_thresholdSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TemplateMatchNode::onThresholdChanged);
}

TemplateMatchNode::~TemplateMatchNode()
{
    if (_templateDialog) {
        _templateDialog->deleteLater();
    }
}

unsigned int TemplateMatchNode::nPorts(QtNodes::PortType type) const
{
    if (type == QtNodes::PortType::In)
        return 1;
    else
        return 2;  // Port 0: Image, Port 1: MatchResult
}

QtNodes::NodeDataType TemplateMatchNode::dataType(QtNodes::PortType type, QtNodes::PortIndex port) const
{
    if (type == QtNodes::PortType::In)
    {
        return {"image", "Image"};
    }
    else
    {
        if (port == 0)
            return {"image", "Image"};
        else
            return {"match_result", "MatchResult"};
    }
}

void TemplateMatchNode::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port)
{
    if (!FlowExecutionContext::running.load())
        return;

    auto img = std::dynamic_pointer_cast<ImageData>(data);
    if (!img)
        return;

    auto input = img->matPtr();
    if (!input || input->empty())
        return;

    _inputImage = input;
}

std::shared_ptr<QtNodes::NodeData> TemplateMatchNode::outData(QtNodes::PortIndex port)
{
    if (port == 0)
    {
        // 图像输出
        if (!_inputImage || _inputImage->empty())
            return nullptr;

        return std::make_shared<ImageData>(_inputImage);
    }
    else if (port == 1)
    {
        // 匹配结果输出
        if (_matchResults.isEmpty())
            return nullptr;

        auto& result = _matchResults.first();
        QMap<QString, QVariant> resultMap;
        resultMap["pixelX"] = result.pixelX;
        resultMap["pixelY"] = result.pixelY;
        resultMap["angle"] = result.angle;
        resultMap["score"] = result.score;
        resultMap["width"] = result.matchRect.width;
        resultMap["height"] = result.matchRect.height;

        return std::make_shared<MatchResultData>(resultMap);
    }

    return nullptr;
}

QVariant TemplateMatchNode::getOutput(int port)
{
    if (port == 0)
    {
        // 图像数据 - FlowExecutor 使用
        if (!_inputImage || _inputImage->empty())
            return QVariant();

        return QVariant::fromValue(_inputImage);
    }
    else if (port == 1)
    {
        // 匹配结果 - FlowExecutor 使用
        if (_matchResults.isEmpty())
            return QVariant();

        auto& result = _matchResults.first();
        QMap<QString, QVariant> resultMap;
        resultMap["pixelX"] = result.pixelX;
        resultMap["pixelY"] = result.pixelY;
        resultMap["angle"] = result.angle;
        resultMap["score"] = result.score;
        resultMap["width"] = result.matchRect.width;
        resultMap["height"] = result.matchRect.height;

        return QVariant::fromValue(resultMap);
    }

    return QVariant();
}

void TemplateMatchNode::setInput(int port, const QVariant &data)
{
    if (port == 0)
    {
        if (!data.isValid()) {
            _inputImage.reset();
            return;
        }
        _inputImage = data.value<MatPtr>();
    }
    // Port 1 是输出端口，不需要 setInput
}

void TemplateMatchNode::compute()
{
    if (!_inputImage || _inputImage->empty()) {
        setStatus(FlowStatus::NodeStatus::Done);
        return;
    }

    if (!_hasTemplate || _templateImage.empty()) {
        qDebug() << "[TemplateMatchNode] No template set!";
        setStatus(FlowStatus::NodeStatus::Done);
        return;
    }

    _matchResults.clear();
    cv::Mat src = *_inputImage;
    cv::Mat result;

    cv::matchTemplate(src, _templateImage, result, cv::TM_CCOEFF_NORMED);

    cv::Point maxLoc;
    double maxVal;
    cv::minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);

    if (maxVal >= _matchThreshold) {
        MatchResult match;
        match.pixelX = maxLoc.x + _templateRect.width / 2.0f;
        match.pixelY = maxLoc.y + _templateRect.height / 2.0f;
        match.angle = 0.0f;
        match.score = maxVal;
        match.matchRect = cv::Rect(maxLoc.x, maxLoc.y, _templateRect.width, _templateRect.height);
        _matchResults.append(match);
    }

    setStatus(FlowStatus::NodeStatus::Done);

    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);
}

QJsonObject TemplateMatchNode::saveData() const
{
    QJsonObject obj;
    obj["hasTemplate"] = _hasTemplate;
    obj["threshold"] = _matchThreshold;

    if (_hasTemplate && !_templateImage.empty()) {
        std::vector<uchar> buf;
        cv::imencode(".png", _templateImage, buf);
        QByteArray ba(reinterpret_cast<const char*>(buf.data()), buf.size());
        obj["templateImage"] = QString(ba.toBase64());
        obj["templateWidth"] = _templateRect.width;
        obj["templateHeight"] = _templateRect.height;
    }

    return obj;
}

void TemplateMatchNode::loadData(const QJsonObject &obj)
{
    _hasTemplate = obj["hasTemplate"].toBool();
    _matchThreshold = obj["threshold"].toDouble(0.8);
    _thresholdSpin->setValue(_matchThreshold);

    if (_hasTemplate && obj.contains("templateImage")) {
        QString base64 = obj["templateImage"].toString();
        QByteArray ba = QByteArray::fromBase64(base64.toUtf8());
        std::vector<uchar> buf(ba.begin(), ba.end());
        _templateImage = cv::imdecode(buf, cv::IMREAD_COLOR);

        int w = obj["templateWidth"].toInt(0);
        int h = obj["templateHeight"].toInt(0);
        _templateRect = cv::Rect(0, 0, w, h);

        if (!_templateImage.empty()) {
            _templateInfoLabel->setText(QString("Template: %1 x %2").arg(w).arg(h));
            _templateInfoLabel->setStyleSheet("color: green;");
        }
    }
}

std::vector<cv::Mat> TemplateMatchNode::outputImages() const
{
    if (!_inputImage || _inputImage->empty())
        return {};

    cv::Mat display = _inputImage->clone();

    for (const auto& result : _matchResults) {
        cv::rectangle(display, result.matchRect, cv::Scalar(0, 255, 0), 2);
        cv::circle(display, cv::Point2f(result.pixelX, result.pixelY), 5, cv::Scalar(0, 0, 255), -1);

        int crossSize = 10;
        cv::line(display,
            cv::Point(result.pixelX - crossSize, result.pixelY),
            cv::Point(result.pixelX + crossSize, result.pixelY),
            cv::Scalar(0, 0, 255), 1);
        cv::line(display,
            cv::Point(result.pixelX, result.pixelY - crossSize),
            cv::Point(result.pixelX, result.pixelY + crossSize),
            cv::Scalar(0, 0, 255), 1);

        QString infoText = QString("X:%1 Y:%2 Angle:%3 Score:%4")
            .arg(result.pixelX, 0, 'f', 1)
            .arg(result.pixelY, 0, 'f', 1)
            .arg(result.angle, 0, 'f', 1)
            .arg(result.score, 0, 'f', 2);

        cv::putText(display, infoText.toStdString(),
            cv::Point(result.matchRect.x, result.matchRect.y - 10),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }

    if (!_hasTemplate) {
        cv::putText(display, "No Template Set",
            cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
    }

    return { display };
}

void TemplateMatchNode::onCreateTemplateClicked()
{
    if (!_templateDialog) {
        _templateDialog = new TemplateMatchDialog(nullptr);
    }

    if (_inputImage && !_inputImage->empty()) {
        _templateDialog->setImage(*_inputImage);
    }

    _templateDialog->exec();

    if (_templateDialog->hasSelection()) {
        _templateImage = _templateDialog->getTemplateImage();
        _templateRect = _templateDialog->getTemplateRect();
        _hasTemplate = true;

        _templateInfoLabel->setText(QString("Template: %1 x %2")
            .arg(_templateRect.width).arg(_templateRect.height));
        _templateInfoLabel->setStyleSheet("color: green;");
    }
}

void TemplateMatchNode::onThresholdChanged(double value)
{
    _matchThreshold = value;
}

REGISTER_NODE(TemplateMatchNode, "Image")
