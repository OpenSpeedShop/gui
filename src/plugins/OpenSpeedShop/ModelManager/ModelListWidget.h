#ifndef MODELLISTWIDGET_H
#define MODELLISTWIDGET_H

#include <QWidget>
#include <QDomDocument>
#include <QUuid>
#include <QMap>
#include <QList>

#include "ModelDescriptor.h"

class QTreeWidgetItem;

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui { class ModelListWidget; }

class ModelListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModelListWidget(QWidget *parent = 0);
    ~ModelListWidget();

    QString filter() const;
    void setFilter(const QString &filter);

signals:
    void currentModelDescriptorChanged(ModelDescriptor *);

protected:
    void setModelDescriptors(const QList<ModelDescriptor *> &modelDescriptors);
    void buildModelDescriptorTree(const QString &filter, const QList<ModelDescriptor *> &modelDescriptors);
    void refreshTreeView();

    QString m_Filter;
    QList<ModelDescriptor *> m_ModelDescriptors;
    QMap<QString, QList<ModelDescriptor *> *> m_SortedModelDescriptors;

protected slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::ModelListWidget *ui;

    friend class ModelManager;
};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // MODELLISTWIDGET_H
