#ifndef MODULE_LIBRARY_WIDGET_H
#define MODULE_LIBRARY_WIDGET_H
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>

// 前向声明
class ModuleTreeWidget;

class ModuleLibraryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleLibraryWidget(QWidget *parent = nullptr);
    ~ModuleLibraryWidget();

    void refreshModuleList();

signals:
    void moduleDoubleClicked(const QString& moduleType);
    void moduleDragStarted(const QString& moduleType);

private slots:
    void onSearchTextChanged(const QString& text);
    void onItemDoubleClicked(const QString& moduleType);
    void onItemDragStarted(const QString& moduleType, const QPixmap& pixmap);

private:
    void setupUI();
    void populateModules();

private:
    ModuleTreeWidget* m_treeWidget;
    QLineEdit* m_searchEdit;
};
#endif // MODULE_LIBRARY_WIDGET_H
