#include "ModelDescriptorListWidget.h"

#include <QStandardItem>
#include "ModelDescriptor.h"

namespace Plugins {
namespace OpenSpeedShop {

ModelDescriptorListWidget::ModelDescriptorListWidget(QAbstractItemModel *descriptorsModel, QWidget *parent) :
    QTreeView(parent)
{
    QTreeView::setModel(new QSortFilterProxyModel(this));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSortingEnabled(true);

    setModel(descriptorsModel);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleClicked(QModelIndex)));
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(itemSingleClicked(QModelIndex)));
}

ModelDescriptorListWidget::~ModelDescriptorListWidget()
{
}

QString ModelDescriptorListWidget::filter() const
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QTreeView::model());
    return proxyModel->filterRegExp().pattern();
}

void ModelDescriptorListWidget::setFilter(const QString &regex)
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QTreeView::model());
    proxyModel->setFilterRegExp(regex);
    proxyModel->setFilterKeyColumn(0);
    proxyModel->setFilterRole(Qt::DisplayRole);
}

QString ModelDescriptorListWidget::experimentType() const
{
    return rootIndex().data(Qt::DisplayRole).toString();
}

void ModelDescriptorListWidget::setExperimentType(const QString &experimentType)
{
    //NOTE: QAbstractItemModel::match() seems to search children, we only want root level items searched!
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QTreeView::model());

    if(!experimentType.isEmpty()) {
        for(int row=0; row < proxyModel->rowCount(QModelIndex()); row++) {
            QModelIndex index = proxyModel->index(row, 0, QModelIndex());
            if(!index.data(Qt::DisplayRole).toString().compare(experimentType, Qt::CaseInsensitive)) {
                setRootIndex(index);
                setRootIsDecorated(false);
                return;
            }
        }
    }

    setRootIndex(QModelIndex());
    setRootIsDecorated(true);
    expandAll();
}

QAbstractItemModel *ModelDescriptorListWidget::model() const
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QTreeView::model());
    return proxyModel->sourceModel();
}

void ModelDescriptorListWidget::setModel(QAbstractItemModel *model)
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QTreeView::model());
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    expandAll();
}

void ModelDescriptorListWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    QModelIndexList selectedRows;
    foreach(QModelIndex index, selectedIndexes) {
        if(index.column() == 0) {
            selectedRows.append(index);
        }
    }

    if(selectedRows.count() != 1) {
        emit currentDescriptorChanged(QUuid());
        return;
    }

    QModelIndex selectedRow = selectedRows.at(0);

    QUuid uid(selectedRow.data(Qt::UserRole).toString());
    emit currentDescriptorChanged(uid);

    QTreeView::selectionChanged(selected, deselected);
}

void ModelDescriptorListWidget::itemSingleClicked(QModelIndex index)
{
    QString uidString = index.data(Qt::UserRole).toString();
    if(!uidString.isEmpty()) {
        emit descriptorSingleClicked(QUuid(uidString));
    }
}

void ModelDescriptorListWidget::itemDoubleClicked(QModelIndex index)
{
    QString uidString = index.data(Qt::UserRole).toString();
    if(!uidString.isEmpty()) {
        emit descriptorDoubleClicked(QUuid(uidString));
    }
}


void ModelDescriptorListWidget::selectRow(const QUuid &uid)
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    if(selectionModel) {
        selectionModel->clear();
        QModelIndex selectedIndex = findIndex(uid);
        if(selectedIndex.isValid()) {

            QModelIndex selectedIndexParent = model()->parent(selectedIndex);
            if(selectedIndexParent.isValid()) {
                expand(selectedIndexParent);
            }

            selectionModel->setCurrentIndex(selectedIndex, QItemSelectionModel::SelectCurrent);
        }
    }
}

QModelIndex ModelDescriptorListWidget::findIndex(const QUuid &uid, const QModelIndex &parent) const
{
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(QTreeView::model());

    QModelIndex modelIndex;
    for(int row = 0; row < proxyModel->rowCount(parent); row++) {
        QModelIndex modelIndex = proxyModel->index(row, 0, parent);
        if(modelIndex.isValid() && QUuid(modelIndex.data(Qt::UserRole).toString()) == uid) {
            return modelIndex;
        }
        findIndex(uid, modelIndex);
    }

    return modelIndex;
}

} // namespace OpenSpeedShop
} // namespace Plugins
