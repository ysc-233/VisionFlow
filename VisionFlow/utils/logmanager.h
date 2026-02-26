#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QDateTime>
#include <QDir>
#include <atomic>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4
};

struct LogMessage {
    LogLevel level;
    QString message;
    QString timestamp;
    QString threadId;
};

class LogWriterThread : public QThread {
    Q_OBJECT
public:
    explicit LogWriterThread(QObject *parent = nullptr);
    ~LogWriterThread();

    void enqueueMessage(const LogMessage &msg);
    void stop();

signals:
    void messageReady(const QString &formattedMsg, LogLevel level);

protected:
    void run() override;

private:
    QQueue<LogMessage> m_messageQueue;
    QMutex m_queueMutex;
    QMutex m_fileMutex;
    std::atomic<bool> m_running{true};
    QFile m_logFile;
    QTextStream m_logStream;

    QString m_logDir;
    QString m_currentLogFile;
    qint64 m_currentFileSize = 0;
    static constexpr qint64 MAX_FILE_SIZE = 50 * 1024 * 1024; // 50MB

    void initLogFile();
    void rotateFile();
    void writeToFile(const LogMessage &msg);
    QString formatMessage(const LogMessage &msg);
};


class LogManager : public QObject {
    Q_OBJECT
public:
    static LogManager* getInstance();
    static void destroyInstance();

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator=(LogManager&&) = delete;

    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);
    void fatal(const QString &message);

    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;

    QString getLogDirPath() const;

signals:
    void logMessageReceived(const QString &formattedMessage, LogLevel level);

public slots:
    void cleanupOldLogs(int keepDays = 7);

private:
    explicit LogManager(QObject *parent = nullptr);
    ~LogManager();

    void postLog(LogLevel level, const QString &message);
    QString getThreadId() const;
    QString generateTimestamp() const;

    static LogManager* m_instance;
    static QMutex m_instanceMutex;

    LogWriterThread *m_writerThread;
    LogLevel m_logLevel;
    QMutex m_levelMutex;
};

// 便捷宏定义
#define LOG_DEBUG(msg) LogManager::getInstance()->debug(msg)
#define LOG_INFO(msg) LogManager::getInstance()->info(msg)
#define LOG_WARNING(msg) LogManager::getInstance()->warning(msg)
#define LOG_ERROR(msg) LogManager::getInstance()->error(msg)
#define LOG_FATAL(msg) LogManager::getInstance()->fatal(msg)

#endif // LOGMANAGER_H
