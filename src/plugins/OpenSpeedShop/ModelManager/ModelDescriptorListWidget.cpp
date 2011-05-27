#include "ModelDescriptorListWidget.h"
#include "ui_ModelDescriptorListWidget.h"

#include "ModelDescriptor.h"

#include <QStandardItem>

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
}

QString ModelDescriptorListWidget::experimentType() const
{
    return ui->treeView->rootIndex().data(Qt::DisplayRole).toString();
}

void ModelDescriptorListWidget::setExperimentType(const QString &experimentType)
{
    //NOTE: QAbstractItemModel::match() seems to search children, we only want root level items searched!

    if(!experimentType.isEmpty()) {
        for(int row=0; row < m_DescriptorsModel->rowCount(QModelIndex()); row++) {
            QModelIndex index = m_DescriptorsModel->index(row, 0, QModelIndex());
            if(!index.data(Qt::DisplayRole).toString().compare(experimentType, Qt::CaseInsensitive)) {
                ui->treeView->setRootIndex(index);
                ui->treeView->setRootIsDecorated(false);
                return;
            }
        }
    }

    ui->treeView->setRootIndex(QModelIndex());
    ui->treeView->setRootIsDecorated(true);
    ui->treeView->expandAll();
}

void ModelDescriptorListWidget::setModel(QAbstractItemModel *descriptorsModel)
{
    m_DescriptorsModel->setSourceModel(descriptorsModel);
    m_DescriptorsModel->setDynamicSortFilter(true);         // Re sort and filter if the source model changes
    ui->treeView->setModel(m_DescriptorsModel);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->expandAll();

    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

void ModelDescriptorListWidget::selectionChanged(QItemSelection selected, QItemSelection deselected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    QModelIndexList selectedRows;
    foreach(QModelIndex index, selectedIndexes) {
        if(index.column() == 0) {
            selectedRows.append(index);
        }
    }

    if(selectedRows.count() != 1) {
        emit currentSelectionChanged(QUuid());
        return;
    }

    QModelIndex selectedRow = selectedRows.at(0);

    QUuid uid(selectedRow.data(Qt::UserRole).toString());
    emit currentSelectionChanged(uid);
}

void ModelDescriptorListWidget::on_treeView_doubleClicked(QModelIndex index)
{
    QUuid descriptorUid(index.data(Qt::UserRole).toString());
    emit doubleClicked(descriptorUid);
}


void ModelDescriptorListWidget::selectRow(const QUuid &uid)
{
    QItemSelectionModel *selectionModel = ui->treeView->selectionModel();
    if(selectionModel) {
        selectionModel->clear();
        QModelIndex selectedIndex = findIndex(uid);
        if(selectedIndex.isValid()) {
            selectionModel->setCurrentIndex(selectedIndex, QItemSelectionModel::SelectCurrent);
        }
    }
}

QModelIndex ModelDescriptorListWidget::findIndex(const QUuid &uid, const QModelIndex &parent) const
{
    QModelIndex modelIndex;
    for(int row = 0; row < m_DescriptorsModel->rowCount(parent); row++) {
        QModelIndex modelIndex = m_DescriptorsModel->index(row, 0, parent);
        if(modelIndex.isValid() && QUuid(modelIndex.data(Qt::UserRole).toString()) == uid) {
            return modelIndex;
        }
        findIndex(uid, modelIndex);
    }

    return modelIndex;
}


} // namespace OpenSpeedShop
} // namespace Plugins
