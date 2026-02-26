#include "logmanager.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QDirIterator>
#include <QtDebug>
#include <QString>

// 静态成员初始化
LogManager* LogManager::m_instance = nullptr;
QMutex LogManager::m_instanceMutex;

// ============================================================================
// LogWriterThread 实现
// ============================================================================

LogWriterThread::LogWriterThread(QObject *parent)
    : QThread(parent)
{
    // 创建日志目录
    m_logDir = QCoreApplication::applicationDirPath() + "/VisionFlowLog";
    QDir dir(m_logDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    initLogFile();
}

LogWriterThread::~LogWriterThread()
{
    stop();
    if (isRunning()) {
        wait(3000); // 等待最多 3 秒
    }
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void LogWriterThread::initLogFile()
{
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss") + ".log";
    m_currentLogFile = m_logDir + "/" + fileName;

    if (m_logFile.isOpen()) {
        m_logFile.close();
    }

    m_logFile.setFileName(m_currentLogFile);
    if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_logStream.setDevice(&m_logFile);
        m_logStream.setCodec("UTF-8");
        m_currentFileSize = m_logFile.size();
    } else {
        qCritical() << "Failed to open log file:" << m_currentLogFile;
    }
}

void LogWriterThread::rotateFile()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }

    // 重命名当前文件（添加序号）
    QFileInfo fileInfo(m_currentLogFile);
    QString baseName = fileInfo.baseName();
    QString rotationName = m_logDir + "/" + baseName + "_archived.log";

    // 如果已存在，添加时间戳
    if (QFile::exists(rotationName)) {
        rotationName = m_logDir + "/" + baseName + "_" +
                       QDateTime::currentDateTime().toString("HH-mm-ss") + "_archived.log";
    }

    QFile::rename(m_currentLogFile, rotationName);

    // 创建新文件
    initLogFile();
}

void LogWriterThread::enqueueMessage(const LogMessage &msg)
{
    QMutexLocker locker(&m_queueMutex);
    m_messageQueue.enqueue(msg);
}

void LogWriterThread::stop()
{
    m_running = false;
}

QString LogWriterThread::formatMessage(const LogMessage &msg)
{
    QString levelStr;
    switch (msg.level) {
        case LogLevel::DEBUG:   levelStr = "DEBUG";   break;
        case LogLevel::INFO:    levelStr = "INFO";    break;
        case LogLevel::WARNING: levelStr = "WARNING"; break;
        case LogLevel::ERROR:   levelStr = "ERROR";   break;
        case LogLevel::FATAL:   levelStr = "FATAL";   break;
    }

    return QString("[%1][%2][Thread:%3] %4")
        .arg(msg.timestamp)
        .arg(levelStr)
        .arg(msg.threadId)
        .arg(msg.message);
}

void LogWriterThread::writeToFile(const LogMessage &msg)
{
    QMutexLocker locker(&m_fileMutex);

    if (m_currentFileSize >= MAX_FILE_SIZE) {
        rotateFile();
    }

    QString formattedMsg = formatMessage(msg);

    if (m_logFile.isOpen()) {
        m_logStream << formattedMsg << "\n";
        m_logStream.flush();
        m_currentFileSize = m_logFile.size();
    }

    emit messageReady(formattedMsg, msg.level);
}

void LogWriterThread::run()
{
    while (m_running) {
        LogMessage msg;
        bool hasMessage = false;

        {
            QMutexLocker locker(&m_queueMutex);
            if (!m_messageQueue.isEmpty()) {
                msg = m_messageQueue.dequeue();
                hasMessage = true;
            }
        }

        if (hasMessage) {
            writeToFile(msg);
        } else {
            msleep(10);
        }
    }

    QMutexLocker locker(&m_queueMutex);
    while (!m_messageQueue.isEmpty()) {
        LogMessage msg = m_messageQueue.dequeue();
        writeToFile(msg);
    }
}

LogManager::LogManager(QObject *parent)
    : QObject(parent)
    , m_logLevel(LogLevel::DEBUG)
{
    m_writerThread = new LogWriterThread(this);

    connect(m_writerThread, &LogWriterThread::messageReady,
            this, &LogManager::logMessageReceived,
            Qt::QueuedConnection);

    m_writerThread->start();
}

LogManager::~LogManager()
{
    m_writerThread->stop();
    m_writerThread->wait(3000);
}

LogManager* LogManager::getInstance()
{
    QMutexLocker locker(&m_instanceMutex);
    if (!m_instance) {
        m_instance = new LogManager();
    }
    return m_instance;
}

void LogManager::destroyInstance()
{
    QMutexLocker locker(&m_instanceMutex);
    if (m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

void LogManager::postLog(LogLevel level, const QString &message)
{
    {
        QMutexLocker locker(&m_levelMutex);
        if (level < m_logLevel) {
            return;
        }
    }

    LogMessage msg;
    msg.level = level;
    msg.message = message;
    msg.timestamp = generateTimestamp();
    msg.threadId = getThreadId();

    m_writerThread->enqueueMessage(msg);
}

void LogManager::debug(const QString &message)
{
    postLog(LogLevel::DEBUG, message);
}

void LogManager::info(const QString &message)
{
    postLog(LogLevel::INFO, message);
}

void LogManager::warning(const QString &message)
{
    postLog(LogLevel::WARNING, message);
}

void LogManager::error(const QString &message)
{
    postLog(LogLevel::ERROR, message);
}

void LogManager::fatal(const QString &message)
{
    postLog(LogLevel::FATAL, message);
}

void LogManager::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_levelMutex);
    m_logLevel = level;
}

LogLevel LogManager::getLogLevel() const
{
    QMutexLocker locker(const_cast<QMutex*>(&m_levelMutex));
    return m_logLevel;
}

QString LogManager::getLogDirPath() const
{
    return QCoreApplication::applicationDirPath() + "/VisionFlowLog";
}

QString LogManager::generateTimestamp() const
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd-HH:mm:ss");
}

QString LogManager::getThreadId() const
{
    return QString::number(quintptr(QThread::currentThreadId()));
}

void LogManager::cleanupOldLogs(int keepDays)
{
    QDir dir(getLogDirPath());
    QFileInfoList files = dir.entryInfoList(QStringList() << "*.log",
                                             QDir::Files,
                                             QDir::Time);

    QDateTime threshold = QDateTime::currentDateTime().addDays(-keepDays);

    for (const QFileInfo &file : files) {
        if (file.lastModified() < threshold) {
            QFile::remove(file.filePath());
            info(QString("Cleaned old log file: %1").arg(file.fileName()));
        }
    }
}
