#include "property_editor_widget.h"

#include "core/flow_node.h"

PropertyEditorWidget::PropertyEditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentNode(nullptr)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_groupBox = new QGroupBox("属性编辑器", this);
    mainLayout->addWidget(m_groupBox);

    m_formLayout = new QFormLayout(m_groupBox);
}

PropertyEditorWidget::~PropertyEditorWidget()
{
}

void PropertyEditorWidget::editNode(const FlowNode* node)
{
    clearForm();
    m_currentNode = node;

    if (!node) {
        QLabel* label = new QLabel("未选择节点", this);
        m_formLayout->addRow(label);
        return;
    }

    // 显示节点基本信息
    QLineEdit* idEdit = new QLineEdit(node->id(), this);
    idEdit->setReadOnly(true);
    addPropertyRow("节点ID:", idEdit);

    QLineEdit* typeEdit = new QLineEdit(node->moduleType(), this);
    typeEdit->setReadOnly(true);
    addPropertyRow("模块类型:", typeEdit);

    // 显示节点参数
    auto parameters = node->parameters();
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        QVariant value = it.value();

        QWidget* editor = nullptr;

        if (value.type() == QVariant::Int) {
            QSpinBox* spinBox = new QSpinBox(this);
            spinBox->setValue(value.toInt());
            editor = spinBox;
        }
        else if (value.type() == QVariant::Double) {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
            spinBox->setValue(value.toDouble());
            editor = spinBox;
        }
        else if (value.type() == QVariant::Bool) {
            QCheckBox* checkBox = new QCheckBox(this);
            checkBox->setChecked(value.toBool());
            editor = checkBox;
        }
        else if (value.type() == QVariant::StringList) {
            QComboBox* comboBox = new QComboBox(this);
            comboBox->addItems(value.toStringList());
            editor = comboBox;
        }
        else {
            QLineEdit* lineEdit = new QLineEdit(value.toString(), this);
            editor = lineEdit;
        }

        if (editor) {
            addPropertyRow(it.key() + ":", editor);
        }
    }

    if (parameters.isEmpty()) {
        QLabel* label = new QLabel("this node has no parameter", this);
        m_formLayout->addRow(label);
    }
}

void PropertyEditorWidget::clear()
{
    clearForm();
}

void PropertyEditorWidget::clearForm()
{
    // 删除所有行
    while (m_formLayout->rowCount() > 0) {
        m_formLayout->removeRow(0);
    }
    m_currentNode = nullptr;
}

void PropertyEditorWidget::addPropertyRow(const QString& label, QWidget* widget)
{
    QLabel* labelWidget = new QLabel(label, this);
    m_formLayout->addRow(labelWidget, widget);
}
