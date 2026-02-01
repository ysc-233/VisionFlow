#ifndef PROPERTY_EDITOR_WIDGET_H
#define PROPERTY_EDITOR_WIDGET_H
#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

class FlowNode;

class PropertyEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyEditorWidget(QWidget *parent = nullptr);
    ~PropertyEditorWidget();

    void editNode(const FlowNode* node);
    void clear();

private:
    void clearForm();
    void addPropertyRow(const QString& label, QWidget* widget);

private:
    QFormLayout* m_formLayout;
    QGroupBox* m_groupBox;
    const FlowNode* m_currentNode;
};


#endif // PROPERTY_EDITOR_WIDGET_H
