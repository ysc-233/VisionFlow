#include "log_output_widget.h"
#include <QDateTime>
#include <QDebug>
#include "log_output_widget.h"
#include <QDateTime>
#include <QDebug>

LogOutputWidget::LogOutputWidget(QWidget *parent)
    : QWidget(parent)
    , m_autoScroll(true)
{
    setupUI();
}

LogOutputWidget::~LogOutputWidget()
{
}

void LogOutputWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建工具栏
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(5, 5, 5, 5);

    m_btnClear = new QPushButton("清除", this);
    connect(m_btnClear, &QPushButton::clicked, this, &LogOutputWidget::onClearClicked);
    toolbarLayout->addWidget(m_btnClear);
    toolbarLayout->addStretch();

    mainLayout->addLayout(toolbarLayout);

    // 创建文本编辑框
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setFont(QFont("Consolas", 9));
    mainLayout->addWidget(m_textEdit);
}

void LogOutputWidget::logMessage(const QString& message, QtMsgType type)
{
    if (!m_textEdit) return;

    // 格式化消息
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString levelStr;

    switch (type) {
    case QtDebugMsg: levelStr = "DEBUG"; break;
    case QtInfoMsg: levelStr = "INFO"; break;
    case QtWarningMsg: levelStr = "WARN"; break;
    case QtCriticalMsg: levelStr = "ERROR"; break;
    default: levelStr = "INFO"; break;
    }

    QString formattedMsg = QString("[%1][%2] %3").arg(timestamp).arg(levelStr).arg(message);

    // 设置颜色
    QTextCharFormat format;
    format.setForeground(msgTypeToColor(type));

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(formattedMsg + "\n", format);

    // 自动滚动
    if (m_autoScroll) {
        m_textEdit->ensureCursorVisible();
    }

    // 输出到控制台（用于调试）
    switch (type) {
    case QtDebugMsg: qDebug() << message; break;
    case QtInfoMsg: qInfo() << message; break;
    case QtWarningMsg: qWarning() << message; break;
    case QtCriticalMsg: qCritical() << message; break;
    default: qInfo() << message; break;
    }
}

void LogOutputWidget::clear()
{
    if (m_textEdit) {
        m_textEdit->clear();
    }
}

void LogOutputWidget::setAutoScroll(bool enabled)
{
    m_autoScroll = enabled;
}

void LogOutputWidget::onClearClicked()
{
    clear();
}

QColor LogOutputWidget::msgTypeToColor(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg: return QColor(100, 100, 100);    // 灰色
    case QtInfoMsg: return QColor(0, 0, 0);           // 黑色
    case QtWarningMsg: return QColor(255, 165, 0);    // 橙色
    case QtCriticalMsg: return QColor(255, 0, 0);     // 红色
    default: return Qt::black;
    }
}
