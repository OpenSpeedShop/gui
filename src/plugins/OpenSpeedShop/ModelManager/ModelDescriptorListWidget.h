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
    void currentModelDescriptorChanged(const QUuid&);

protected:
    void setModel(QAbstractItemModel *descriptorsModel);
    QSortFilterProxyModel *m_DescriptorsModel;

protected slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::ModelDescriptorListWidget *ui;
};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // MODELDESCRIPTORLISTWIDGET_H
