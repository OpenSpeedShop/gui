#include "ModelListWidget.h"
#include "ui_ModelListWidget.h"

#include <QFile>
#include <QTreeWidgetItem>

namespace Plugins {
namespace OpenSpeedShop {

ModelListWidget::ModelListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelListWidget)
{
    ui->setupUi(this);
}

ModelListWidget::~ModelListWidget()
{
    delete ui;
}

QString ModelListWidget::filter() const
{
    return m_Filter;
}

void ModelListWidget::setFilter(const QString &filter)
{
    m_Filter = filter;
    buildModelDescriptorTree(m_Filter, m_ModelDescriptors);
}

void ModelListWidget::buildModelDescriptorTree(const QString &filter, const QList<ModelDescriptor *> &modelDescriptors)
{
    QRegExp rxFilter(filter);
    foreach(ModelDescriptor *modelDescriptor, modelDescriptors) {
        QString experimentType = modelDescriptor->experimentType();
        if(rxFilter.exactMatch(experimentType)) {
            if(!m_SortedModelDescriptors.keys().contains(experimentType)) {
                m_SortedModelDescriptors[experimentType] = new QList<ModelDescriptor *>();
            }
            m_SortedModelDescriptors[experimentType]->append(modelDescriptor);
        }
    }

    refreshTreeView();
}

void ModelListWidget::refreshTreeView() {
    foreach(QString experimentType, m_SortedModelDescriptors.keys()) {
        QTreeWidgetItem *experimentTypeItem = new QTreeWidgetItem(ui->treeWidget);
        experimentTypeItem->setData(0, Qt::DisplayRole, experimentType);
        ui->treeWidget->addTopLevelItem(experimentTypeItem);

        foreach(ModelDescriptor *modelDescriptor, *m_SortedModelDescriptors[experimentType]) {
            QTreeWidgetItem *modelDescriptorItem = new QTreeWidgetItem(experimentTypeItem);
            modelDescriptorItem->setData(0, Qt::DisplayRole, modelDescriptor->name());
            modelDescriptorItem->setData(0, Qt::UserRole, modelDescriptor->id().toString());
            experimentTypeItem->addChild(modelDescriptorItem);
        }
    }

    // If there's only one root item, set it as the sole root item (good for filtered results)
    if(m_SortedModelDescriptors.count() == 1) {
        QModelIndex rootIndex = ui->treeWidget->model()->index(0,0);
        ui->treeWidget->setRootIndex(rootIndex);
    }
}

void ModelListWidget::setModelDescriptors(const QList<ModelDescriptor *> &modelDescriptors)
{
    m_ModelDescriptors = modelDescriptors;
    buildModelDescriptorTree(m_Filter, m_ModelDescriptors);
}

void ModelListWidget::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    QUuid id(current->data(0, Qt::UserRole).toString());
    foreach(ModelDescriptor *modelDescriptor, m_ModelDescriptors) {
        if(modelDescriptor->id() == id) {
            emit currentModelDescriptorChanged(modelDescriptor);
            return;
        }
    }
}


} // namespace OpenSpeedShop
} // namespace Plugins
