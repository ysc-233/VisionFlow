#include "floweditorwidget.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QTabBar>
#include <QJsonArray>
#include <QProgressDialog>

#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include "flow/executor/flowrunner.h"
#include "nodes/imageloadnode.h"
#include "nodes/flownodeautoregister.h"
#include "flowview.h"
#include "flowscene.h"
#include "customtabwidget.h"
#include "utils/imageconvert.h"
FlowEditorWidget::FlowEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    using namespace QtNodes;
    m_flowNode = new ImageLoadNode();//临时初始化

    m_registry = std::make_shared<NodeDelegateModelRegistry>();
    NodeAutoRegistry::instance().registerAll(*m_registry);

    m_tabWidget = new CustomTabWidget(this);
    m_tabWidget->setTabsClosable(false);
    m_tabWidget->setMovable(true);

    connect(static_cast<CustomTabWidget*>(m_tabWidget), &CustomTabWidget::addButtonClicked,
            this, [this](){
        int newIndex = m_tabWidget->count();
        createFlowPage(QString("Flow%1").arg(newIndex));
    });

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_tabWidget);

    createFlowPage("Flow1");

    setupTabContextMenu();

    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index){
        if (m_tabWidget->count() == 1) {
            if (QMessageBox::question(this, "Close Tab",
                                      "This is the last tab. Do you want to clear all nodes instead?",
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                auto page = currentPage();
                if (page && page->model) {
                    auto nodeIds = page->model->allNodeIds();
                    for (auto nodeId : nodeIds) {
                        page->model->deleteNode(nodeId);
                    }
                }
            }
        } else {
            removeTab(index);
        }
    });

    connect(&FlowRunner::instance(),&FlowRunner::sig_runFinished,this,&FlowEditorWidget::updateDisplay);
}

FlowEditorWidget::~FlowEditorWidget()
{
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        removeTab(i);
    }
}

FlowEditorWidget::FlowPage* FlowEditorWidget::createFlowPage(QString name)
{
    using namespace QtNodes;

    QWidget* container = new QWidget(this);
    auto lay = new QVBoxLayout(container);
    lay->setContentsMargins(0,0,0,0);

    auto model = new DataFlowGraphModel(m_registry);
    model->setParent(container);

    auto scene = new FlowScene(*model, m_registry, container);
    auto view = new FlowView(scene, container);

    view->setRenderHint(QPainter::Antialiasing);
    lay->addWidget(view);

    int index = m_tabWidget->addTab(container, name);
    m_tabWidget->setCurrentIndex(index);

    FlowPage* page = new FlowPage{ model, scene, view, container };
    container->setProperty("flowpage", QVariant::fromValue((void*)page));

    connect(scene, &FlowScene::nodeSelected,this, [=](QtNodes::NodeId nodeId)
    {
        auto items = scene->selectedItems();
        if (items.isEmpty())
            return;

        auto page = currentPage();
        if (!page) return;

        auto* delegate = page->model->delegateModel<QtNodes::NodeDelegateModel>(nodeId);

        FlowNode* fn = dynamic_cast<FlowNode*>(delegate);
        if (!fn) return;
        m_flowNode = fn;
        updateDisplay();
    });

    return page;
}

FlowEditorWidget::FlowPage* FlowEditorWidget::currentPage()
{
    QWidget* w = m_tabWidget->currentWidget();
    if (!w)
        return nullptr;

    return static_cast<FlowPage*>(w->property("flowpage").value<void*>());
}

void FlowEditorWidget::removeTab(int index)
{
    if (index < 0 || index >= m_tabWidget->count())
        return;

    QWidget* w = m_tabWidget->widget(index);
    if (!w)
        return;
    FlowPage* page = static_cast<FlowPage*>(w->property("flowpage").value<void*>());
    delete page;
    m_tabWidget->removeTab(index);
}

void FlowEditorWidget::setupTabContextMenu()
{
    m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_tabWidget,
            &QTabWidget::customContextMenuRequested,
            this,
            [this](QPoint pos)
    {
        int index = m_tabWidget->tabBar()->tabAt(pos);

        if (index < 0)
            return;

        QMenu menu;
        QAction* rename = menu.addAction("Rename Tab");
        menu.addSeparator();
        QAction* remove = menu.addAction("Delete Tab");
        menu.addSeparator();
        QAction* duplicate = menu.addAction("Duplicate Tab");


        QAction* act = menu.exec(m_tabWidget->mapToGlobal(pos));

        if (act == rename) {
            bool ok;
            QString text = QInputDialog::getText(
                        this, "Rename Tab",
                        "New tab name:",
                        QLineEdit::Normal,
                        m_tabWidget->tabText(index),
                        &ok);

            if (ok && !text.isEmpty())
                m_tabWidget->setTabText(index, text);
        }
        else if (act == remove) {
            if (QMessageBox::question(this, "Delete Tab",
                                      QString("Delete tab '%1' and all its contents?").arg(m_tabWidget->tabText(index)),
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

                removeTab(index);
            }
        }
        else if (act == duplicate) {
            auto page = currentPage();
            if (page) {
                QString newName = QString("Copy of %1").arg(m_tabWidget->tabText(index));
                auto newPage = createFlowPage(newName);
                // newPage->model->load(page->model->save());
            }
        }
    });
}

void FlowEditorWidget::updateDisplay()
{
    auto imgs = m_flowNode->outputImages();
    if (imgs.empty())
        return;

    int width = 0;
    int height = 0;

    for (auto& m : imgs)
    {
        width = std::max(width, m.cols);
        height += m.rows;
    }

    cv::Mat result(height, width, imgs[0].type(), cv::Scalar::all(0));

    int y = 0;
    for (auto& m : imgs)
    {
        m.copyTo(result(cv::Rect(0, y, m.cols, m.rows)));
        y += m.rows;
    }
    QImage qimg = MatToQImage(result);
    emit sig_updateImage(qimg);
}


void FlowEditorWidget::newFlow()
{
    int result = QMessageBox::question(
                this,
                "New Workspace",
                "Create new workspace? Current unsaved changes will be lost.",
                QMessageBox::Yes | QMessageBox::No);

    if (result != QMessageBox::Yes) {
        return;
    }
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        removeTab(i);
    }
    createFlowPage("Flow1");
}

void FlowEditorWidget::saveFlow(const QString &file)
{
    QJsonObject root;
    QJsonArray flows;

    for (int i = 0; i < m_tabWidget->count(); ++i)
    {
        QWidget* w = m_tabWidget->widget(i);
        if (!w)
            continue;

        FlowPage* page =
            static_cast<FlowPage*>(w->property("flowpage").value<void*>());

        if (!page || !page->model)
            continue;

        QJsonObject flowObj;
        flowObj["name"] = m_tabWidget->tabText(i);
        flowObj["graph"] = page->model->save();

        QJsonObject customNodes;

        for (auto nodeId : page->model->allNodeIds())
        {
            auto* delegate =
                page->model->delegateModel<QtNodes::NodeDelegateModel>(nodeId);

            if (!delegate)
                continue;

            FlowNode* fn =
                dynamic_cast<FlowNode*>(delegate);

            if (!fn)
                continue;

            QJsonObject custom = fn->saveData();

            if (!custom.isEmpty())
            {
                customNodes[QString::number(nodeId)] = custom;
            }
        }

        if (!customNodes.isEmpty())
            flowObj["nodesCustom"] = customNodes;

        flows.append(flowObj);
    }

    root["flows"] = flows;

    QFile f(file);
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(QJsonDocument(root).toJson());
        f.close();

        QMessageBox::information(this, "Save",
            QString("Workflow saved to:\n%1").arg(file));
    }
    else
    {
        QMessageBox::critical(this, "Save Error",
            QString("Failed to save file:\n%1").arg(file));
    }
}

void FlowEditorWidget::loadFlow(const QString &file)
{
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Load Error",
            QString("Failed to open file:\n%1").arg(file));
        return;
    }

    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
    {
        QMessageBox::critical(this, "Load Error",
            "Invalid JSON format");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray flows = root["flows"].toArray();

    for (int i = m_tabWidget->count() - 1; i >= 0; --i)
        removeTab(i);

    for (int i = 0; i < flows.size(); ++i)
    {
        QJsonObject flowObj = flows[i].toObject();
        QString name = flowObj["name"].toString();
        QJsonObject graph = flowObj["graph"].toObject();
        QJsonObject customNodes =
            flowObj["nodesCustom"].toObject();

        auto page = createFlowPage(name);
        if (!page || !page->model)
            continue;

        page->model->load(graph);

        for (auto nodeId : page->model->allNodeIds())
        {
            QString idStr = QString::number(nodeId);

            if (!customNodes.contains(idStr))
                continue;

            auto* delegate =
                page->model->delegateModel<QtNodes::NodeDelegateModel>(nodeId);

            if (!delegate)
                continue;

            FlowNode* fn =
                dynamic_cast<FlowNode*>(delegate);

            if (!fn)
                continue;

            fn->loadData(customNodes[idStr].toObject());
        }
    }

    QMessageBox::information(this, "Load",
        QString("Workflow loaded from:\n%1").arg(file));
}
