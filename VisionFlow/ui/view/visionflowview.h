#ifndef VISIONFLOWVIEW_H
#define VISIONFLOWVIEW_H

#include <QWidget>
#include <QMenuBar>
#include "ui/component/WorkSpace/floweditorwidget.h"
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
    void createMenu();
private:
    Ui::VisionFlowView *ui;
    FlowEditorWidget *m_flowEditorWidget;

    QMenuBar* m_menuBar;
    QMenu* m_fileMenu;

    QAction* m_actNew;
    QAction* m_actOpen;
    QAction* m_actSave;
    QAction* m_actSaveAs;

    QString m_currentFile;
};

#endif // VISIONFLOWVIEW_H
