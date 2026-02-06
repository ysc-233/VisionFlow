#ifndef VISIONFLOWVIEW_H
#define VISIONFLOWVIEW_H

#include <QWidget>
#include "ui/component/floweditorwidget.h"
namespace Ui {
class VisionFlowView;
}

class VisionFlowView : public QWidget
{
    Q_OBJECT

public:
    explicit VisionFlowView(QWidget *parent = nullptr);
    ~VisionFlowView();
private:
    void setConnections();
private:
    Ui::VisionFlowView *ui;
    FlowEditorWidget *m_flowEditorWidget;
};

#endif // VISIONFLOWVIEW_H
