#ifndef TEMPLATEMATCHNODE_H
#define TEMPLATEMATCHNODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include "flow/executor/flowexecutioncontext.h"
#include "flow/graph/flownode.h"
#include "flow/flowtypes.h"
#include <QMap>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QDoubleSpinBox>
#include <opencv2/opencv.hpp>

class TemplateMatchDialog;

// ============================================================================
// 匹配结果 NodeData 类型（用于 QtNodes 数据流 Port 1）
// ============================================================================
class MatchResultData : public QtNodes::NodeData
{
public:
    MatchResultData() = default;

    MatchResultData(const QMap<QString, QVariant>& result)
        : _result(result)
    {}

    QtNodes::NodeDataType type() const override
    {
        return {"match_result", "MatchResult"};
    }

    QMap<QString, QVariant> result() const
    {
        return _result;
    }

private:
    QMap<QString, QVariant> _result;
};

// ============================================================================
// 模板匹配节点
// ============================================================================
class TemplateMatchNode : public QtNodes::NodeDelegateModel, public FlowNode
{
    Q_OBJECT
public:
    static QString staticNodeName;
    TemplateMatchNode();
    ~TemplateMatchNode() override;

    QString caption() const override { return "Template Match"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return staticNodeName; }

    unsigned int nPorts(QtNodes::PortType type) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port) override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

    QWidget* embeddedWidget() override { return _widget; }

    // ✅ FlowNode 接口 - 两个端口都要实现
    QVariant getOutput(int port) override;
    void setInput(int port, const QVariant &data) override;

    void compute() override;

    QJsonObject saveData() const override;
    void loadData(const QJsonObject &obj) override;

    std::vector<cv::Mat> outputImages() const override;

private slots:
    void onCreateTemplateClicked();
    void onThresholdChanged(double value);

private:
    QWidget* _widget = nullptr;
    QPushButton* _btnCreateTemplate = nullptr;
    QDoubleSpinBox* _thresholdSpin = nullptr;
    QLabel* _templateInfoLabel = nullptr;

    MatPtr _inputImage = nullptr;
    cv::Mat _templateImage;
    cv::Rect _templateRect;
    bool _hasTemplate = false;
    double _matchThreshold = 0.8;

    struct MatchResult {
        float pixelX;
        float pixelY;
        float angle;
        float score;
        cv::Rect matchRect;
    };
    QList<MatchResult> _matchResults;

    TemplateMatchDialog* _templateDialog = nullptr;
};

// ============================================================================
// 模板选择视图
// ============================================================================
class TemplateSelectionView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TemplateSelectionView(QWidget* parent = nullptr);
    ~TemplateSelectionView() override;

    void setImage(const cv::Mat& img);
    cv::Rect getSelectionRect() const;
    void clearSelection();

signals:
    void selectionReleased();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    cv::Mat _image;
    QPointF _startPos;
    QPointF _endPos;
    bool _isSelecting = false;
    QGraphicsRectItem* _selectionRect = nullptr;
    QGraphicsScene* _scene = nullptr;
};

// ============================================================================
// 模板创建对话框
// ============================================================================
class TemplateMatchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TemplateMatchDialog(QWidget* parent = nullptr);
    ~TemplateMatchDialog() override;

    void setImage(const cv::Mat& img);
    cv::Mat getTemplateImage() const;
    cv::Rect getTemplateRect() const;
    bool hasSelection() const;

private slots:
    void onLoadImageClicked();
    void onSaveClicked();
    void onCancelClicked();

private:
    cv::Mat _sourceImage;
    cv::Mat _templateImage;
    cv::Rect _templateRect;
    bool _hasSelection = false;

    TemplateSelectionView* _view = nullptr;
    QLabel* _imageLabel = nullptr;
    QPushButton* _btnLoad = nullptr;
    QPushButton* _btnSave = nullptr;
    QPushButton* _btnCancel = nullptr;
};


#endif // TEMPLATEMATCHNODE_H
