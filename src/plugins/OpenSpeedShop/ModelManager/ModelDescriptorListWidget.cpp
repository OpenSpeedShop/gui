#include "ModelDescriptorListWidget.h"
#include "ui_ModelDescriptorListWidget.h"


#include "ModelDescriptor.h"

namespace Plugins {
namespace OpenSpeedShop {

ModelDescriptorListWidget::ModelDescriptorListWidget(QAbstractItemModel *descriptorsModel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelDescriptorListWidget)
{
    m_DescriptorsModel = new QSortFilterProxyModel(this);
    ui->setupUi(this);

    setModel(descriptorsModel);
}

ModelDescriptorListWidget::~ModelDescriptorListWidget()
{
    delete ui;
}

QString ModelDescriptorListWidget::filter() const
{
    return m_DescriptorsModel->filterRegExp().pattern();
}

void ModelDescriptorListWidget::setFilter(const QString &regex)
{
    m_DescriptorsModel->setFilterRegExp(regex);
    m_DescriptorsModel->setFilterKeyColumn(0);
    m_DescriptorsModel->setFilterRole(Qt::DisplayRole);

    //DEBUG: I don't know if this will actually work.  We may have to count the filtered rows by some other method.
    // Collapse tree if there's only one root item
    if(m_DescriptorsModel->rowCount() <= 1) {
        ui->treeView->setRootIndex(m_DescriptorsModel->index(0,0));
    } else {
        ui->treeView->setRootIndex(QModelIndex());
    }
}

void ModelDescriptorListWidget::setModel(QAbstractItemModel *descriptorsModel)
{
    m_DescriptorsModel->setSourceModel(descriptorsModel);
    m_DescriptorsModel->setDynamicSortFilter(true);         // Re sort and filter if the source model changes
    ui->treeView->setModel(m_DescriptorsModel);
    ui->treeView->setSortingEnabled(true);

    //DEBUG: I don't know if this will actually work.  We may have to count the filtered rows by some other method.
    // Collapse tree if there's only one root item
    if(m_DescriptorsModel->rowCount() <= 1) {
        ui->treeView->setRootIndex(m_DescriptorsModel->index(0,0));
    } else {
        ui->treeView->setRootIndex(QModelIndex());
    }
}

void ModelDescriptorListWidget::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    QUuid id(current->data(0, Qt::UserRole).toString());
    emit currentModelDescriptorChanged(id);
}

} // namespace OpenSpeedShop
} // namespace Plugins
