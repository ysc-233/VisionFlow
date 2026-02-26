#ifndef VISIONFLOWVIEW_H
#define VISIONFLOWVIEW_H

#include <QWidget>
#include <QMenuBar>
#include "utils/logmanager.h"
#include "ui/component/WorkSpace/floweditorwidget.h"

struct LogEntry {
    QString rawMessage;
    QString formattedHtml;
    LogLevel level;
    qint64 timestamp;
};

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

    void refreshLogDisplay();
    QString highlightKeyword(const QString &text, const QString &keyword);

private slots:
    void onLogMessageReceived(const QString &message, LogLevel level);
    void onSearchTextChanged(const QString &text);

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

    //Log
    QList<LogEntry> m_logEntries;
    QString m_currentFilterKeyword;
    QTimer *m_filterDebounceTimer;
    static constexpr int FILTER_DEBOUNCE_MS = 150;
};

#endif // VISIONFLOWVIEW_H
