#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <QObject>
#include <QDomDocument>

#include <QStandardItemModel>
#include <QUuid>

namespace Plugins {
namespace OpenSpeedShop {

class ModelDescriptor;
class ModelDescriptorWidget;
class ModelDescriptorListWidget;
class DataModel;

class ModelManager : public QObject
{
    Q_OBJECT
public:
    static ModelManager *instance();

    void initialize();
    void shutdown();

    void fetchDescriptors();
    void importDescriptors(const QString &filepath = QString());
    void exportDescriptors(const QString &filepath = QString());
    QUuid createDescriptor();
    void removeDescriptor(const QUuid &descriptorId);
    ModelDescriptorWidget *createDescriptorWidget(const QUuid &descriptorId, QWidget *parent = NULL);
    ModelDescriptorListWidget *createDescriptorListWidget(QWidget *parent = NULL);
    QMimeData descriptorMimeData(const QUuid &descriptorId);

    QUuid importModel(const QString &filepath = QString());
    void exportModel(const QUuid &modelId, const QString &filepath = QString());
    QUuid fetchModel(const QUuid &descriptorId, const QUuid &experimentId);
    void unloadModel(const QUuid &modelId);
    void unloadModel(const QUuid &descriptorId, const QUuid &experimentId);
    QMimeData modelMimeData(const QUuid &modelId);
    QMimeData modelMimeData(const QUuid &descriptorId, const QUuid &experimentId);
    QAbstractItemModel *model(const QUuid &modelId);
    QAbstractItemModel *model(const QUuid &descriptorId, const QUuid &experimentId);
    QUuid modelId(const QUuid &descriptorId, const QUuid &experimentId) const;

protected:
    explicit ModelManager(QObject *parent = 0);

    void insertDescriptorIntoModel(QUuid descriptorId);
    ModelDescriptor *descriptor(QUuid descriptorId);
    QAbstractItemModel *descriptorModel();

    QHash<QUuid, ModelDescriptor *> m_DescriptorPool;
    QHash<QUuid, QStandardItem *> m_DescriptorToItem;
    QStandardItemModel m_DescriptorPoolModel;

    // DescriptorID -> ExperimentID -> ModelID
    QMultiHash<QUuid, QHash<QUuid, QUuid>*> m_ModelLookupTable;
    QHash<QUuid, DataModel*> m_ModelPool;

protected slots:
    void descriptorDataChanged();

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // MODELMANAGER_H
