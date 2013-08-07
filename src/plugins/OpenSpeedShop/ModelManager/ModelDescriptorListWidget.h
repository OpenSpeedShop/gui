#ifndef PLUGINS_OPENSPEEDSHOP_MODELDESCRIPTORLISTWIDGET_H
#define PLUGINS_OPENSPEEDSHOP_MODELDESCRIPTORLISTWIDGET_H

#include <QTreeView>
#include <QUuid>
class QSortFilterProxyModel;

#include "ModelManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class ModelDescriptor;

class MODELMANAGER_EXPORT ModelDescriptorListWidget : public QTreeView
{
    Q_OBJECT

public:
    explicit ModelDescriptorListWidget(QAbstractItemModel *descriptorsModel, QWidget *parent = 0);
    ~ModelDescriptorListWidget();

    QString filter() const;
    void setFilter(const QString &regex);

    QString experimentType() const;
    void setExperimentType(const QString &experimentType);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    QSortFilterProxyModel *proxyModel() const;

signals:
    void currentDescriptorChanged(const QUuid &descriptorUid);
    void descriptorSingleClicked(const QUuid &descriptorUid);
    void descriptorDoubleClicked(const QUuid &descriptorUid);

public slots:
    void selectRow(const QUuid &uid);
    void selectRow(int row);
    void selectDefault();

protected:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    QModelIndex findIndex(const QUuid &uid, const QModelIndex &parent = QModelIndex()) const;
    QString m_ExperimentType;

protected slots:
    void itemSingleClicked(QModelIndex index);
    void itemDoubleClicked(QModelIndex index);

};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // PLUGINS_OPENSPEEDSHOP_MODELDESCRIPTORLISTWIDGET_H
