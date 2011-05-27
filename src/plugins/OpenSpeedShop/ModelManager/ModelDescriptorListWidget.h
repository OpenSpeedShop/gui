#ifndef MODELDESCRIPTORLISTWIDGET_H
#define MODELDESCRIPTORLISTWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDomDocument>
#include <QUuid>
#include <QMap>
#include <QList>
#include <QFile>
#include <QTreeWidgetItem>
#include <QSortFilterProxyModel>

namespace Plugins {
namespace OpenSpeedShop {

class ModelDescriptor;
namespace Ui { class ModelDescriptorListWidget; }

class ModelDescriptorListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModelDescriptorListWidget(QAbstractItemModel *descriptorsModel, QWidget *parent = 0);
    ~ModelDescriptorListWidget();

    QString filter() const;
    void setFilter(const QString &regex);

signals:
    void currentSelectionChanged(const QUuid&);

public slots:
    void selectRow(const QUuid &uid);

protected:
    void setModel(QAbstractItemModel *descriptorsModel);
    QSortFilterProxyModel *m_DescriptorsModel;
    QModelIndex findIndex(const QUuid &uid, const QModelIndex &parent = QModelIndex()) const;

protected slots:
    void selectionChanged(QItemSelection selected, QItemSelection deselected);

private:
    Ui::ModelDescriptorListWidget *ui;
};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // MODELDESCRIPTORLISTWIDGET_H
