#ifndef MAIN_EDITOR_WIDGET_H
#define MAIN_EDITOR_WIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QToolBar>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

// 前向声明核心类
class FlowGraph;
class FlowWorkspaceWidget;
class ModuleLibraryWidget;
class PropertyEditorWidget;
class LogOutputWidget;

/**
 * @brief 主编辑器窗口
 *
 * 包含五个主要组件：
 * 1. 工具栏 (groupBox_MainToolBar)
 * 2. 模块库 (groupBox_moduleLibrary)
 * 3. 工作区 (tabWidget_workspaces)
 * 4. 属性编辑器 (groupBox_propertyEditor)
 * 5. 日志输出 (groupBox_logOutput)
 */
class MainEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainEditorWidget(QWidget *parent = nullptr);
    ~MainEditorWidget();

    // 获取当前活动的工作区
    FlowWorkspaceWidget* currentWorkspace() const;

    // 获取当前活动的流程图
    FlowGraph* currentFlowGraph() const;

    // 文件操作
    bool newFlow();
    bool saveFlow();
    bool openFlow(const QString& filePath = QString());

    void testDragAndDrop();

public slots:
    void onRunTriggered();
    void onStopTriggered();
    void onNewFlowTriggered();
    void onOpenFlowTriggered();
    void onSaveFlowTriggered();

    // 日志相关
    void logInfo(const QString& message);
    void logWarning(const QString& message);
    void logError(const QString& message);
    void logDebug(const QString& message);

    // 工作区管理
    void onTabChanged(int index);
    void onTabCloseRequested(int index);

    // 模块拖放
    void onModuleDragRequested(const QString& moduleType);
    void onModuleDoubleClicked(const QString& moduleType);

signals:
    void executionStarted();
    void executionFinished(bool success);
    void flowModified(bool modified);
    void nodeSelected(const class FlowNode* node);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    // 初始化函数
    void setupUI();
    void setupToolBar();
    void setupConnections();

    // 辅助函数
    void createNewTab(const QString& title, FlowGraph* graph = nullptr);
    QPointF calculateNodePosition(FlowWorkspaceWidget* workspace);
    bool promptSaveIfModified();
    void updateWindowTitle();
    void updateRecentFiles();
    void addModuleToWorkspace(const QString& moduleType, const QPointF& pos = QPointF());
    // UI组件
    QVBoxLayout* m_mainLayout;

    // 五个主要组件
    QGroupBox* m_groupBoxToolBar;      // 工具栏
    QGroupBox* m_groupBoxModuleLibrary; // 模块库
    QTabWidget* m_tabWidgetWorkspaces; // 工作区标签页
    QGroupBox* m_groupBoxPropertyEditor; // 属性编辑器
    QGroupBox* m_groupBoxLogOutput;    // 日志输出

    // 分割器
    QSplitter* m_mainSplitter;         // 主水平分割器
    QSplitter* m_rightSplitter;        // 右侧垂直分割器

    // 自定义组件（稍后实现）
    QToolBar* m_toolBar;               // 工具栏
    ModuleLibraryWidget* m_moduleLibraryWidget; // 模块库组件
    PropertyEditorWidget* m_propertyEditorWidget; // 属性编辑器组件
    LogOutputWidget* m_logOutputWidget; // 日志输出组件

    // 工作区列表
    QList<FlowWorkspaceWidget*> m_workspaces;
    QList<FlowGraph*> m_flowGraphs;

    // 状态管理
    QStatusBar* m_statusBar;
    QStringList m_recentFiles;
    QStringList m_tabFilePaths;
    QList<bool> m_tabModified;

    // 动作
    QAction* m_actionRun;
    QAction* m_actionStop;
    QAction* m_actionNew;
    QAction* m_actionOpen;
    QAction* m_actionSave;

    bool m_isRunning;
};


#endif // MAIN_EDITOR_WIDGET_H
