#include "visionflowview.h"
#include "ui_visionflowview.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include "flow/executor/flowrunner.h"
#include "ui/component/NodeLibrary/nodelibrarywidget.h"


VisionFlowView::VisionFlowView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisionFlowView)
{
    ui->setupUi(this);

    LogManager *logMgr = LogManager::getInstance();
    logMgr->setLogLevel(LogLevel::DEBUG); // 设置日志级别
    // 定期清理旧日志（7 天）
    QTimer::singleShot(60000, logMgr, [logMgr]() {logMgr->cleanupOldLogs(7);});
    LOG_INFO("VisionFlow Start");
    LOG_DEBUG("LogManager finished initialized!");

    m_flowEditorWidget = new FlowEditorWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(ui->gpb_workspace);
    lay->addWidget(m_flowEditorWidget);

    NodeLibraryWidget *m_nodeLibraryWidget = new NodeLibraryWidget(this);
    QVBoxLayout *lay_nodeLib = new QVBoxLayout(ui->gpb_nodeLibrary);
    lay_nodeLib->addWidget(m_nodeLibraryWidget);

    ui->led_logSearch->setPlaceholderText("Search..");
    createMenu();
    setConnections();
}

VisionFlowView::~VisionFlowView()
{
    delete ui;
}

void VisionFlowView::setConnections()
{
    connect(ui->btn_run, &QPushButton::clicked,this, [this]()
    {
        auto page = m_flowEditorWidget->currentPage();
        if (page && page->model) {
            FlowRunner::instance().run(*page->model);
        }
    });

    connect(ui->btn_Stop, &QPushButton::clicked,this, [this]()
    {
        FlowRunner::instance().stop();
    });

    connect(m_flowEditorWidget, &FlowEditorWidget::sig_updateImage,this, [=](QImage img)
    {
        QPixmap pixmap = QPixmap::fromImage(img);
        ui->lbl_display->setPixmap(pixmap);
        ui->lbl_display->setPixmap(pixmap.scaled(ui->lbl_display->size(), Qt::KeepAspectRatio));
    });

    connect(LogManager::getInstance(), &LogManager::logMessageReceived,
            this, &VisionFlowView::onLogMessageReceived,
            Qt::QueuedConnection);


    connect(ui->btn_ClearLog, &QPushButton::clicked,this, [this]()
    {
        ui->ted_logTextEdit->clear();
    });

    m_filterDebounceTimer = new QTimer(this);
    m_filterDebounceTimer->setSingleShot(true);
    connect(m_filterDebounceTimer, &QTimer::timeout, this, [this]() {
        onSearchTextChanged(ui->led_logSearch->text());
    });

    connect(ui->led_logSearch, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_filterDebounceTimer->start(FILTER_DEBOUNCE_MS);
    });

}

void VisionFlowView::createMenu()
{
    m_menuBar = new QMenuBar(this);

    // File 菜单
    m_fileMenu = m_menuBar->addMenu(tr("File"));
    m_actNew    = new QAction(tr("New"), this);
    m_actOpen   = new QAction(tr("Open"), this);
    m_actSave   = new QAction(tr("Save"), this);
    m_actSaveAs = new QAction(tr("Save As"), this);

    m_fileMenu->addAction(m_actNew);
    m_fileMenu->addAction(m_actOpen);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_actSave);
    m_fileMenu->addAction(m_actSaveAs);

    // 添加到预留的菜单容器
    ui->layout_menu->layout()->addWidget(m_menuBar);

    // 设置菜单栏样式，使其占满宽度
    m_menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_actNew, &QAction::triggered, this, [this]() {
        m_flowEditorWidget->newFlow();
        m_currentFile.clear();
    });

    connect(m_actOpen, &QAction::triggered, this, [this]() {
        QString file = QFileDialog::getOpenFileName(
                    this, "Open Workflow", "", "Workflow (*.json)");
        if (!file.isEmpty()) {
            m_flowEditorWidget->loadFlow(file);
            m_currentFile = file;
        }
    });

    connect(m_actSave, &QAction::triggered, this, [this]() {
        if (m_currentFile.isEmpty()) {
            m_actSaveAs->trigger();
            return;
        }
        m_flowEditorWidget->saveFlow(m_currentFile);
    });

    connect(m_actSaveAs, &QAction::triggered, this, [this]() {
        QString file = QFileDialog::getSaveFileName(
                    this, "Save Workflow", "", "Workflow (*.json)");
        if (!file.isEmpty()) {
            m_flowEditorWidget->saveFlow(file);
            m_currentFile = file;
        }
    });
}

void VisionFlowView::refreshLogDisplay()
{
    ui->ted_logTextEdit->clear();

    if (m_logEntries.isEmpty()) {
        return;
    }

    if (m_currentFilterKeyword.isEmpty())
    {
        for (const auto &entry : m_logEntries) {
            ui->ted_logTextEdit->append(entry.formattedHtml);
        }
    }
    else
    {
        for (const auto &entry : m_logEntries)
        {
            if (entry.rawMessage.contains(m_currentFilterKeyword, Qt::CaseInsensitive))
            {
                QString highlighted = highlightKeyword(entry.formattedHtml, m_currentFilterKeyword);
                ui->ted_logTextEdit->append(highlighted);
            }
        }
    }
    auto *scrollBar = ui->ted_logTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

QString VisionFlowView::highlightKeyword(const QString &text, const QString &keyword)
{
    if (keyword.isEmpty())
    {
        return text;
    }

    QString escapedKeyword = QRegularExpression::escape(keyword);

    QRegularExpression regex("(" + escapedKeyword + ")", QRegularExpression::CaseInsensitiveOption);

    QString highlightStyle = "<span style='background-color:#ffd700; color:#000000; font-weight:bold;'>\\1</span>";

    QString result = text;
    return result.replace(regex, highlightStyle);
}

void VisionFlowView::onLogMessageReceived(const QString &message, LogLevel level)
{
    LogEntry entry;
    entry.rawMessage = message;
    entry.level = level;
    entry.timestamp = QDateTime::currentMSecsSinceEpoch();

    QString color;
    switch (level) {
    case LogLevel::DEBUG:   color = "#808080"; break;
    case LogLevel::INFO:    color = "#4ec9b0"; break;
    case LogLevel::WARNING: color = "#dcdcaa"; break;
    case LogLevel::ERROR:   color = "#f48771"; break;
    case LogLevel::FATAL:   color = "#ff0000"; break;
    }
    entry.formattedHtml = QString("<span style='color:%1;'>%2</span>").arg(color, message);

    m_logEntries.append(entry);

    if (!m_currentFilterKeyword.isEmpty())
    {
        if (!message.contains(m_currentFilterKeyword, Qt::CaseInsensitive))
        {
            return;
        }
        QString highlighted = highlightKeyword(entry.formattedHtml, m_currentFilterKeyword);
        ui->ted_logTextEdit->append(highlighted);
    }
    else
    {
        ui->ted_logTextEdit->append(entry.formattedHtml);
    }

    auto *scrollBar = ui->ted_logTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void VisionFlowView::onSearchTextChanged(const QString &text)
{
    m_currentFilterKeyword = text.trimmed();
    refreshLogDisplay();
}
