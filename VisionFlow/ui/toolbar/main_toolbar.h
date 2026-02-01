#ifndef MAIN_TOOLBAR_H
#define MAIN_TOOLBAR_H
#include <QWidget>
#include <QToolBar>

QT_BEGIN_NAMESPACE
class QAction;
class QToolButton;
class QComboBox;
QT_END_NAMESPACE

namespace Ui {
class MainToolBar;
}

class MainToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit MainToolBar(QWidget *parent = nullptr);
    ~MainToolBar();

    // 动作获取
    QAction* newAction() const;
    QAction* openAction() const;
    QAction* saveAction() const;
    QAction* saveAsAction() const;
    QAction* runAction() const;
    QAction* stopAction() const;
    QAction* undoAction() const;
    QAction* redoAction() const;
    QAction* cutAction() const;
    QAction* copyAction() const;
    QAction* pasteAction() const;
    QAction* deleteAction() const;
    QAction* selectAllAction() const;
    QAction* zoomInAction() const;
    QAction* zoomOutAction() const;
    QAction* zoomResetAction() const;
    QAction* fitViewAction() const;

    // 状态更新
    void setRunning(bool running);
    void setModified(bool modified);
    void setCanUndo(bool canUndo);
    void setCanRedo(bool canRedo);
    void setZoomLevel(double level);
    void updateRecentFiles(const QStringList& files);

signals:
    void newTriggered();
    void openTriggered();
    void saveTriggered();
    void saveAsTriggered();
    void runTriggered();
    void stopTriggered();
    void undoTriggered();
    void redoTriggered();
    void cutTriggered();
    void copyTriggered();
    void pasteTriggered();
    void deleteTriggered();
    void selectAllTriggered();
    void zoomInTriggered();
    void zoomOutTriggered();
    void zoomResetTriggered();
    void fitViewTriggered();
    void recentFileSelected(const QString& filePath);

public slots:
    void onRecentFileSelected(int index);

private:
    void setupUI();
    void createActions();
    void setupConnections();
    void updateUI();

private:
    Ui::MainToolBar *ui;

    // 文件操作
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_saveAsAction;

    // 编辑操作
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_cutAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_deleteAction;
    QAction* m_selectAllAction;

    // 执行操作
    QAction* m_runAction;
    QAction* m_stopAction;

    // 视图操作
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_zoomResetAction;
    QAction* m_fitViewAction;

    // 控件
    QToolButton* m_fileButton;
    QToolButton* m_editButton;
    QToolButton* m_viewButton;
    QComboBox* m_recentFilesCombo;
    QComboBox* m_zoomCombo;

    bool m_isRunning;
    bool m_isModified;
};

#endif // MAIN_TOOLBAR_H
