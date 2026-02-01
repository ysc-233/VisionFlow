#include "main_editor_widget.h"
#include "ui_main_editor_widget.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QIcon>

#include "ui/module_library/module_library_widget.h"
#include "ui/flow_workspace/flow_workspace_widget.h"
#include "ui/property_editor/property_editor_widget.h"
#include "ui/log_output/log_output_widget.h"

// 核心模块
#include "core/flow_graph.h"
#include "core/flow_node_factory.h"
#include "modules/module_registry.h"
#include "modules/test/register_test_modules.h"

MainEditorWidget::MainEditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_isRunning(false)
{
    // 注册测试模块
    registerTestModules();

    // 设置窗口属性
    setWindowTitle("VisionFlow");
    setMinimumSize(1200, 800);

    // 初始化UI
    setupUI();
    setupConnections();

    // 创建默认流程图
    newFlow();

    // 更新状态栏
    m_statusBar->showMessage("就绪", 3000);
     QTimer::singleShot(1000, this, &MainEditorWidget::testDragAndDrop);
}

MainEditorWidget::~MainEditorWidget()
{
    // 清理所有流程图
    qDeleteAll(m_flowGraphs);
}

// 初始化UI布局
void MainEditorWidget::setupUI()
{
    // 主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(2);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);

    // 1. 创建工具栏分组框
    m_groupBoxToolBar = new QGroupBox(this);
    m_groupBoxToolBar->setTitle("toolbar");
    m_groupBoxToolBar->setMaximumHeight(60);

    QVBoxLayout* toolbarLayout = new QVBoxLayout(m_groupBoxToolBar);
    toolbarLayout->setContentsMargins(5, 15, 5, 5);

    // 创建工具栏
    setupToolBar();
    toolbarLayout->addWidget(m_toolBar);

    m_mainLayout->addWidget(m_groupBoxToolBar);

    // 2. 创建主分割器（水平方向）
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // 2.1 左侧：模块库分组框
    m_groupBoxModuleLibrary = new QGroupBox("module", m_mainSplitter);
    m_groupBoxModuleLibrary->setMinimumWidth(200);
    m_groupBoxModuleLibrary->setMaximumWidth(350);

    QVBoxLayout* moduleLibraryLayout = new QVBoxLayout(m_groupBoxModuleLibrary);
    moduleLibraryLayout->setContentsMargins(5, 15, 5, 5);

    // 创建模块库组件
    m_moduleLibraryWidget = new ModuleLibraryWidget(m_groupBoxModuleLibrary);
    moduleLibraryLayout->addWidget(m_moduleLibraryWidget);

    m_mainSplitter->addWidget(m_groupBoxModuleLibrary);

    // 2.2 中间：工作区和日志（垂直分割器）
    m_rightSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);

    // 2.2.1 上部：工作区标签页
    m_tabWidgetWorkspaces = new QTabWidget(m_rightSplitter);
    m_tabWidgetWorkspaces->setTabsClosable(true);
    m_tabWidgetWorkspaces->setMovable(true);

    m_rightSplitter->addWidget(m_tabWidgetWorkspaces);

    // 2.2.2 下部：日志输出分组框
    m_groupBoxLogOutput = new QGroupBox("log out", m_rightSplitter);
    m_groupBoxLogOutput->setMinimumHeight(150);
    m_groupBoxLogOutput->setMaximumHeight(400);

    QVBoxLayout* logOutputLayout = new QVBoxLayout(m_groupBoxLogOutput);
    logOutputLayout->setContentsMargins(5, 15, 5, 5);

    // 创建日志输出组件
    m_logOutputWidget = new LogOutputWidget(m_groupBoxLogOutput);
    logOutputLayout->addWidget(m_logOutputWidget);

    m_rightSplitter->addWidget(m_groupBoxLogOutput);

    m_mainSplitter->addWidget(m_rightSplitter);

    // 2.3 右侧：属性编辑器分组框
    m_groupBoxPropertyEditor = new QGroupBox("属性编辑器", m_mainSplitter);
    m_groupBoxPropertyEditor->setMinimumWidth(250);
    m_groupBoxPropertyEditor->setMaximumWidth(350);

    QVBoxLayout* propertyEditorLayout = new QVBoxLayout(m_groupBoxPropertyEditor);
    propertyEditorLayout->setContentsMargins(5, 15, 5, 5);

    // 创建属性编辑器组件
    m_propertyEditorWidget = new PropertyEditorWidget(m_groupBoxPropertyEditor);
    propertyEditorLayout->addWidget(m_propertyEditorWidget);

    m_mainSplitter->addWidget(m_groupBoxPropertyEditor);

    // 设置分割器初始比例
    QList<int> mainSizes;
    mainSizes << 250 << 800 << 300;
    m_mainSplitter->setSizes(mainSizes);

    QList<int> rightSizes;
    rightSizes << 600 << 200;
    m_rightSplitter->setSizes(rightSizes);

    m_mainLayout->addWidget(m_mainSplitter, 1); // 第二个参数1表示拉伸因子

    // 3. 创建状态栏
    m_statusBar = new QStatusBar(this);
    m_statusBar->setMinimumHeight(25);
    m_mainLayout->addWidget(m_statusBar);

    // 设置样式
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #cccccc;
            border-radius: 4px;
            margin-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QTabWidget::pane {
            border: 1px solid #cccccc;
            border-radius: 4px;
        }
        QTabBar::tab {
            padding: 6px 12px;
            background-color: #f0f0f0;
            border: 1px solid #cccccc;
            border-bottom: none;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background-color: #ffffff;
        }
        QSplitter::handle {
            background-color: #d0d0d0;
        }
        QSplitter::handle:horizontal {
            width: 2px;
        }
        QSplitter::handle:vertical {
            height: 2px;
        }
    )");
}

// 设置工具栏
void MainEditorWidget::setupToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setIconSize(QSize(24, 24));
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // 创建动作
    m_actionNew = new QAction("new", this);
    m_actionNew->setShortcut(QKeySequence::New);
    m_actionNew->setStatusTip("new flow");

    m_actionOpen = new QAction("open", this);
    m_actionOpen->setShortcut(QKeySequence::Open);
    m_actionOpen->setStatusTip("open flow");

    m_actionSave = new QAction("save", this);
    m_actionSave->setShortcut(QKeySequence::Save);
    m_actionSave->setStatusTip("save flow");

    m_actionRun = new QAction("run", this);
    m_actionRun->setShortcut(Qt::Key_F5);
    m_actionRun->setStatusTip("run flow");

    m_actionStop = new QAction("stop", this);
    m_actionStop->setShortcut(Qt::Key_F6);
    m_actionStop->setStatusTip("stop running");
    m_actionStop->setEnabled(false);

    // 添加到工具栏
    m_toolBar->addAction(m_actionNew);
    m_toolBar->addAction(m_actionOpen);
    m_toolBar->addAction(m_actionSave);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionRun);
    m_toolBar->addAction(m_actionStop);

    // 连接信号
    connect(m_actionNew, &QAction::triggered, this, &MainEditorWidget::onNewFlowTriggered);
    connect(m_actionOpen, &QAction::triggered, this, &MainEditorWidget::onOpenFlowTriggered);
    connect(m_actionSave, &QAction::triggered, this, &MainEditorWidget::onSaveFlowTriggered);
    connect(m_actionRun, &QAction::triggered, this, &MainEditorWidget::onRunTriggered);
    connect(m_actionStop, &QAction::triggered, this, &MainEditorWidget::onStopTriggered);
}

// 设置信号连接
void MainEditorWidget::setupConnections()
{
    // Tab signals
    connect(m_tabWidgetWorkspaces, &QTabWidget::currentChanged,
            this, &MainEditorWidget::onTabChanged);
    connect(m_tabWidgetWorkspaces, &QTabWidget::tabCloseRequested,
            this, &MainEditorWidget::onTabCloseRequested);

    // Module library signals
    if (m_moduleLibraryWidget) {
        // 当模块被双击时
        connect(m_moduleLibraryWidget, &ModuleLibraryWidget::moduleDoubleClicked,
                this, &MainEditorWidget::onModuleDoubleClicked);

        // 连接拖拽信号 - 确保工作区能收到
        connect(m_moduleLibraryWidget, &ModuleLibraryWidget::moduleDragStarted,
                this, &MainEditorWidget::onModuleDragRequested);
    }

    // Execution signals
    connect(this, &MainEditorWidget::executionStarted, [this]() {
        m_isRunning = true;
        m_actionRun->setEnabled(false);
        m_actionStop->setEnabled(true);
        m_statusBar->showMessage("Running...");
    });

    connect(this, &MainEditorWidget::executionFinished, [this](bool success) {
        m_isRunning = false;
        m_actionRun->setEnabled(true);
        m_actionStop->setEnabled(false);
        m_statusBar->showMessage(success ? "Execution completed" : "Execution failed", 3000);
    });
}

QPointF MainEditorWidget::calculateNodePosition(FlowWorkspaceWidget *workspace)
{
    if (!workspace) {
        return QPointF(100, 100); // Default position
    }

    // Get workspace center or use incremental positions
    static int nodeCount = 0;

    // Calculate grid position
    int gridX = nodeCount % 5;  // 5 columns
    int gridY = nodeCount / 5;  // rows

    // Position with some spacing
    qreal x = 100 + gridX * 180;
    qreal y = 100 + gridY * 120;

    nodeCount++;

    return QPointF(x, y);
}

// 获取当前工作区
FlowWorkspaceWidget* MainEditorWidget::currentWorkspace() const
{
    int index = m_tabWidgetWorkspaces->currentIndex();
    if (index >= 0 && index < m_workspaces.size()) {
        return m_workspaces[index];
    }
    return nullptr;
}

// 获取当前流程图
FlowGraph* MainEditorWidget::currentFlowGraph() const
{
    int index = m_tabWidgetWorkspaces->currentIndex();
    if (index >= 0 && index < m_flowGraphs.size()) {
        return m_flowGraphs[index];
    }
    return nullptr;
}

// 新建流程图
bool MainEditorWidget::newFlow()
{
    // 检查是否需要保存当前流程图
    if (!promptSaveIfModified()) {
        return false;
    }

    // 创建新的流程图
    FlowGraph* newGraph = new FlowGraph();

    // 生成标签页名称
    int tabCount = m_tabWidgetWorkspaces->count();
    QString tabName = QString("Flow %1").arg(tabCount + 1);

    // 创建新的工作区
    FlowWorkspaceWidget* workspace = new FlowWorkspaceWidget(newGraph);

    // 添加标签页
    int index = m_tabWidgetWorkspaces->addTab(workspace, tabName);
    m_tabWidgetWorkspaces->setCurrentIndex(index);

    // 保存引用
    m_flowGraphs.append(newGraph);
    m_workspaces.append(workspace);
    m_tabFilePaths.append(QString()); // 空路径表示未保存
    m_tabModified.append(false);

    // 连接工作区信号
    connect(workspace, &FlowWorkspaceWidget::modifiedChanged,
            [this, index](bool modified) {
                if (index >= 0 && index < m_tabModified.size()) {
                    m_tabModified[index] = modified;
                    updateWindowTitle();
                    emit flowModified(modified);
                }
            });

    connect(workspace, &FlowWorkspaceWidget::nodeSelected,
            m_propertyEditorWidget, &PropertyEditorWidget::editNode);

    connect(workspace, &FlowWorkspaceWidget::executionStarted,
            this, &MainEditorWidget::executionStarted);

    connect(workspace, &FlowWorkspaceWidget::executionFinished,
            this, &MainEditorWidget::executionFinished);

    connect(workspace, &FlowWorkspaceWidget::logMessage,
            this, [this](const QString& message, QtMsgType type) {
                switch (type) {
                case QtDebugMsg: logDebug(message); break;
                case QtInfoMsg: logInfo(message); break;
                case QtWarningMsg: logWarning(message); break;
                case QtCriticalMsg: logError(message); break;
                default: logInfo(message); break;
                }
            });

    // 更新窗口标题
    updateWindowTitle();

    // 记录日志
    logInfo(QString("已创建新的流程图: %1").arg(tabName));

    return true;
}

// 保存流程图
bool MainEditorWidget::saveFlow()
{
    FlowGraph* graph = currentFlowGraph();
    if (!graph) {
        logError("没有活动的流程图可以保存");
        return false;
    }

    int index = m_tabWidgetWorkspaces->currentIndex();
    QString filePath = m_tabFilePaths[index];

    if (filePath.isEmpty()) {
        // 弹出保存对话框
        filePath = QFileDialog::getSaveFileName(
            this,
            "save flow",
            QDir::currentPath(),
            "VisionFlow file (*.vflow *.json);;all file (*.*)"
        );

        if (filePath.isEmpty()) {
            return false;
        }
    }

    // TODO: 实现实际的保存逻辑
    // 暂时只标记为已保存
    m_tabFilePaths[index] = filePath;
    m_tabModified[index] = false;

    // 更新标签页标题
    QFileInfo fileInfo(filePath);
    m_tabWidgetWorkspaces->setTabText(index, fileInfo.fileName());

    updateWindowTitle();
    logInfo(QString("已保存到: %1").arg(filePath));

    return true;
}

// 打开流程图
bool MainEditorWidget::openFlow(const QString& filePath)
{
    // 检查是否需要保存当前流程图
    if (!promptSaveIfModified()) {
        return false;
    }

    QString openPath = filePath;
    if (openPath.isEmpty()) {
        openPath = QFileDialog::getOpenFileName(
            this,
            "open flow",
            QDir::currentPath(),
            "VisionFlow file (*.vflow *.json);;all file (*.*)"
        );

        if (openPath.isEmpty()) {
            return false;
        }
    }

    QFile file(openPath);
    if (!file.open(QIODevice::ReadOnly)) {
        logError(QString("无法打开文件: %1").arg(file.errorString()));
        return false;
    }

    // TODO: 实现从文件加载流程图
    // 暂时创建一个新的流程图
    FlowGraph* newGraph = new FlowGraph();

    // 创建新的工作区
    FlowWorkspaceWidget* workspace = new FlowWorkspaceWidget(newGraph);

    // 获取文件名作为标签页名称
    QFileInfo fileInfo(openPath);
    QString tabName = fileInfo.fileName();

    // 添加标签页
    int index = m_tabWidgetWorkspaces->addTab(workspace, tabName);
    m_tabWidgetWorkspaces->setCurrentIndex(index);

    // 保存引用
    m_flowGraphs.append(newGraph);
    m_workspaces.append(workspace);
    m_tabFilePaths.append(openPath);
    m_tabModified.append(false);

    // 连接信号（同newFlow）
    connect(workspace, &FlowWorkspaceWidget::modifiedChanged,
            [this, index](bool modified) {
                if (index >= 0 && index < m_tabModified.size()) {
                    m_tabModified[index] = modified;
                    updateWindowTitle();
                    emit flowModified(modified);
                }
            });

    connect(workspace, &FlowWorkspaceWidget::nodeSelected,
            m_propertyEditorWidget, &PropertyEditorWidget::editNode);

    connect(workspace, &FlowWorkspaceWidget::executionStarted,
            this, &MainEditorWidget::executionStarted);

    connect(workspace, &FlowWorkspaceWidget::executionFinished,
            this, &MainEditorWidget::executionFinished);

    // 更新窗口标题
    updateWindowTitle();

    // 记录日志
    logInfo(QString("已打开文件: %1").arg(openPath));

    return true;
}

void MainEditorWidget::testDragAndDrop()
{
    qDebug() << "=== The drag test start ===";

    // 检查模块库组件
    if (m_moduleLibraryWidget) {
        qDebug() << "modules library exists";
    } else {
        qDebug() << "The module library component does not exist!";
    }

    // 检查工作区组件
    FlowWorkspaceWidget* workspace = currentWorkspace();
    if (workspace) {
        qDebug() << "workspace exists";
        qDebug() << "Whether the workspace accepts drag and drop:" << workspace->acceptDrops();

        // 检查视图
        QGraphicsView* view = workspace->view();
        if (view) {
            qDebug() << "Whether the view accepts drag and drop:" << view->acceptDrops();
            qDebug() << "Whether the viewport accepts drag and drop:" << view->viewport()->acceptDrops();
        }
    } else {
        qDebug() << "The current workspace does not exist!";
    }

    qDebug() << "=== The drag test is over ===";
}

// 槽函数：运行
void MainEditorWidget::onRunTriggered()
{
    FlowWorkspaceWidget* workspace = currentWorkspace();
    if (workspace) {
        logInfo("开始运行流程图...");
        workspace->runFlow();
    } else {
        logWarning("没有活动的工作区可以运行");
    }
}

// 槽函数：停止
void MainEditorWidget::onStopTriggered()
{
    FlowWorkspaceWidget* workspace = currentWorkspace();
    if (workspace) {
        logInfo("正在停止运行...");
        workspace->stopFlow();
    }
}

// 槽函数：新建
void MainEditorWidget::onNewFlowTriggered()
{
    newFlow();
}

// 槽函数：打开
void MainEditorWidget::onOpenFlowTriggered()
{
    openFlow();
}

// 槽函数：保存
void MainEditorWidget::onSaveFlowTriggered()
{
    saveFlow();
}

// 槽函数：标签页改变
void MainEditorWidget::onTabChanged(int index)
{
    if (index >= 0 && index < m_workspaces.size()) {
        // 清空属性编辑器
        m_propertyEditorWidget->clear();

        // 更新窗口标题
        updateWindowTitle();

        // 更新状态栏
        QString tabName = m_tabWidgetWorkspaces->tabText(index);
        m_statusBar->showMessage(QString("当前标签页: %1").arg(tabName), 2000);
    }
}

// 槽函数：关闭标签页
void MainEditorWidget::onTabCloseRequested(int index)
{
    if (index < 0 || index >= m_flowGraphs.size()) {
        return;
    }

    // 检查是否需要保存
    if (m_tabModified[index]) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this,
            "save changed",
            QString("tab \"%1\" finished changed，sure to save?")
                .arg(m_tabWidgetWorkspaces->tabText(index)),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );

        if (result == QMessageBox::Cancel) {
            return;
        } else if (result == QMessageBox::Save) {
            m_tabWidgetWorkspaces->setCurrentIndex(index);
            if (!saveFlow()) {
                return; // 保存失败，不关闭
            }
        }
    }

    // 移除工作区
    FlowWorkspaceWidget* workspace = m_workspaces.takeAt(index);
    FlowGraph* graph = m_flowGraphs.takeAt(index);

    // 移除路径和修改状态
    m_tabFilePaths.removeAt(index);
    m_tabModified.removeAt(index);

    // 删除对象
    delete workspace;
    delete graph;

    // 移除标签页
    m_tabWidgetWorkspaces->removeTab(index);

    // 如果没有标签页了，创建一个新的
    if (m_tabWidgetWorkspaces->count() == 0) {
        newFlow();
    }
}

// 槽函数：模块拖拽请求
void MainEditorWidget::onModuleDragRequested(const QString& moduleType)
{
//    FlowWorkspaceWidget* workspace = currentWorkspace();
//    if (!workspace) {
//        logWarning("No active workspace to add module");
//        return;
//    }

//    // Generate unique node ID
//    static int nodeCounter = 0;
//    QString nodeId = QString("node_%1").arg(++nodeCounter);

//    try {
//        // Create node using factory
//        FlowNode node = FlowNodeFactory::createNode(nodeId, moduleType);

//        // Add to workspace at default position
//        workspace->addNode(node, QPointF(100 + (nodeCounter % 5) * 150,
//                                        100 + (nodeCounter / 5) * 120));
//        addModuleToWorkspace(moduleType);
//        logInfo(QString("Added module: %1 (%2)").arg(nodeId).arg(moduleType));

//    } catch (const std::exception& e) {
//        logError(QString("Failed to create node: %1").arg(e.what()));
//    }
    FlowWorkspaceWidget* workspace = currentWorkspace();
    if (!workspace) {
        logWarning("没有活动的工作区可以添加模块");
        return;
    }

    // 获取鼠标位置
    QPoint globalPos = QCursor::pos();

    // 将全局坐标转换到工作区坐标
    QPoint widgetPos = workspace->mapFromGlobal(globalPos);

    // 将工作区坐标转换到视图坐标
    QGraphicsView* view = workspace->view();  // 需要在 FlowWorkspaceWidget 中添加 getter
    QPoint viewPos = view->mapFromParent(widgetPos);

    // 映射到场景坐标
    QPointF scenePos = view->mapToScene(viewPos);

    // 使用坐标创建节点
    addModuleToWorkspace(moduleType, scenePos);
}

void MainEditorWidget::onModuleDoubleClicked(const QString &moduleType)
{
    FlowWorkspaceWidget* workspace = currentWorkspace();
    if (!workspace) {
        logWarning("No active workspace to add module");
        return;
    }

    // Generate a unique node ID
    static int nodeCounter = 0;
    QString nodeId = QString("node_%1").arg(++nodeCounter);

    try {
        // Create node using factory
        FlowNode node = FlowNodeFactory::createNode(nodeId, moduleType);

        // Calculate position for the new node
        QPointF position = calculateNodePosition(workspace);

        // Add to workspace
        workspace->addNode(node, position);

        // Log success
        logInfo(QString("Added module via double-click: %1 (%2) at position (%3, %4)")
                .arg(nodeId)
                .arg(moduleType)
                .arg(position.x())
                .arg(position.y()));

        // Emit modified signal
        emit flowModified(true);

    } catch (const std::exception& e) {
        logError(QString("Failed to create node from double-click: %1").arg(e.what()));
    } catch (...) {
        logError("Unknown error occurred while creating node");
    }
}

// 日志函数
void MainEditorWidget::logInfo(const QString& message)
{
    if (m_logOutputWidget) {
        m_logOutputWidget->logMessage(message, QtInfoMsg);
    }
    qInfo() << "[INFO]" << message;
}

void MainEditorWidget::logWarning(const QString& message)
{
    if (m_logOutputWidget) {
        m_logOutputWidget->logMessage(message, QtWarningMsg);
    }
    qWarning() << "[WARN]" << message;
}

void MainEditorWidget::logError(const QString& message)
{
    if (m_logOutputWidget) {
        m_logOutputWidget->logMessage(message, QtCriticalMsg);
    }
    qCritical() << "[ERROR]" << message;
}

void MainEditorWidget::logDebug(const QString& message)
{
    if (m_logOutputWidget) {
        m_logOutputWidget->logMessage(message, QtDebugMsg);
    }
    qDebug() << "[DEBUG]" << message;
}

// 关闭事件
void MainEditorWidget::closeEvent(QCloseEvent *event)
{
    // 检查所有标签页是否需要保存
    bool hasUnsavedChanges = false;
    for (int i = 0; i < m_tabModified.size(); ++i) {
        if (m_tabModified[i]) {
            hasUnsavedChanges = true;
            break;
        }
    }

    if (hasUnsavedChanges) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this,
            "save changed",
            "quit?",
            QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel
        );

        if (result == QMessageBox::Cancel) {
            event->ignore();
            return;
        } else if (result == QMessageBox::SaveAll) {
            // 保存所有修改过的标签页
            for (int i = 0; i < m_tabModified.size(); ++i) {
                if (m_tabModified[i]) {
                    m_tabWidgetWorkspaces->setCurrentIndex(i);
                    if (!saveFlow()) {
                        event->ignore();
                        return; // 保存失败，不关闭
                    }
                }
            }
        }
    }

    event->accept();
}

// 提示保存修改
bool MainEditorWidget::promptSaveIfModified()
{
    int currentIndex = m_tabWidgetWorkspaces->currentIndex();
    if (currentIndex < 0 || currentIndex >= m_tabModified.size()) {
        return true;
    }

    if (m_tabModified[currentIndex]) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this,
            "保存更改",
            "当前流程图已修改，是否保存更改？",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );

        if (result == QMessageBox::Cancel) {
            return false;
        } else if (result == QMessageBox::Save) {
            return saveFlow();
        }
    }

    return true;
}

// 更新窗口标题
void MainEditorWidget::updateWindowTitle()
{
    int index = m_tabWidgetWorkspaces->currentIndex();
    if (index < 0) {
        setWindowTitle("VisionFlow");
        return;
    }

    QString tabName = m_tabWidgetWorkspaces->tabText(index);
    QString filePath = m_tabFilePaths[index];

    QString title = QString("VisionFlow - %1").arg(tabName);
    if (m_tabModified[index]) {
        title += " [已修改]";
    }

    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        title += QString(" - %1").arg(fileInfo.fileName());
    }

    setWindowTitle(title);
}

void MainEditorWidget::addModuleToWorkspace(const QString &moduleType, const QPointF &pos)
{
    FlowWorkspaceWidget* workspace = currentWorkspace();
    if (!workspace) {
        logWarning("No active workspace to add module");
        return;
    }

    // Generate a unique node ID
    static int nodeCounter = 0;
    QString nodeId = QString("node_%1").arg(++nodeCounter);

    try {
        // Create node using factory
        FlowNode node = FlowNodeFactory::createNode(nodeId, moduleType);

        // Use provided position or calculate default
        QPointF position = pos;
        if (pos.isNull()) {
            position = calculateNodePosition(workspace);
        }

        // Add to workspace
        workspace->addNode(node, position);

        // Log success
        logInfo(QString("Added module: %1 (%2) at position (%3, %4)")
                .arg(nodeId)
                .arg(moduleType)
                .arg(position.x())
                .arg(position.y()));

        // Emit modified signal
        emit flowModified(true);

    } catch (const std::exception& e) {
        logError(QString("Failed to create node: %1").arg(e.what()));
    } catch (...) {
        logError("Unknown error occurred while creating node");
    }
}
