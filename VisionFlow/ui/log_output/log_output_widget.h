#ifndef LOG_OUTPUT_WIDGET_H
#define LOG_OUTPUT_WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

class LogOutputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogOutputWidget(QWidget *parent = nullptr);
    ~LogOutputWidget();

    void logMessage(const QString& message, QtMsgType type = QtInfoMsg);
    void clear();
    void setAutoScroll(bool enabled);
    bool autoScroll() const { return m_autoScroll; }

public slots:
    void onClearClicked();

private:
    void setupUI();
    QColor msgTypeToColor(QtMsgType type);

private:
    QTextEdit* m_textEdit;
    QPushButton* m_btnClear;
    bool m_autoScroll;
};



#endif // LOG_OUTPUT_WIDGET_H
