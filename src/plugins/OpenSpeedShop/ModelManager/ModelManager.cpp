#include "ModelManager.h"

#include <QApplication>
#include <MainWindow/MainWindow.h>
#include "ConnectionManager/ConnectionManager.h"
#include "ConnectionManager/ServerAdapter.h"
#include "ViewManager/DataModel.h"
#include "ModelManagerDialog.h"
#include "ModelDescriptor.h"
#include "ModelDescriptorWidget.h"
#include "ModelDescriptorListWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

static ModelManager *m_ModelManagerInstance;
ModelManager *ModelManager::instance()
{
    return m_ModelManagerInstance? m_ModelManagerInstance: m_ModelManagerInstance = new ModelManager();
}

ModelManager::ModelManager(QObject *parent) :
    QObject(parent)
{
}

void ModelManager::initialize()
{
    try {
        importDescriptors();
    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(err, NotificationWidget::Critical);
    }

    //DEBUG: Need to get the dialog working
    ModelManagerDialog dlg;
    dlg.exec();
}

void ModelManager::shutdown()
{
    try {
        exportDescriptors();
    } catch(QString err) {
        //TODO: This should probably be a popup, considering the MainWindow is shutting down.
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(err, NotificationWidget::Critical);
    }

    /* self destruct! */
    //FIXME: There should be some handling for the case where instance() is called again after this
    delete(m_ModelManagerInstance);
    m_ModelManagerInstance = NULL;
}

/**********************************************************************************************************/

/*! \brief Loads the user's descriptors from a local XML file and adds them to the descriptor pool. */
void ModelManager::importDescriptors(const QString &filepath)
{
    QString filePath(filepath);
    if(filePath.isEmpty()) {
        //TODO: Create a setting for this in the setting console
        Core::SettingManager::SettingManager *settingManager = Core::SettingManager::SettingManager::instance();
        filePath = QString("%1/ModelDescriptors.xml").arg(QApplication::instance()->applicationDirPath());
        filePath = settingManager->value("Plugins/OpenSpeedShop/ModelManager/DescriptorsFilePath", filePath).toString();
    }

    if(!QFile::exists(filePath)) {
        QFileInfo fileInfo(filePath);
        throw tr("The ModelManager could not load the ModelDescriptor file: %1<br/>"
                       "This file path can be changed in the settings").arg(fileInfo.absoluteFilePath());
    }

    QList<ModelDescriptor *> modelDescriptors = ModelDescriptor::fromXml(filePath, this);

    foreach(ModelDescriptor *modelDescriptor, modelDescriptors) {
        m_DescriptorPool.insert(modelDescriptor->id(), modelDescriptor);
        insertDescriptorIntoModel(modelDescriptor->id());
    }
}

/*! \brief Helper function that adds a descriptor from the descriptor pool into the standard model. */
void ModelManager::insertDescriptorIntoModel(QUuid descriptorId)
{
    //FIXME: This is horribly inefficient.

    ModelDescriptor *descriptor = this->descriptor(descriptorId);
    QList<QStandardItem *> experimentTypeItems = m_DescriptorPoolModel.findItems(descriptor->experimentType());

    // Remove any child items that happened to match
    foreach(QStandardItem *standardItem, experimentTypeItems) {
        if(standardItem->parent()) {
            experimentTypeItems.removeOne(standardItem);
        }
    }

    QStandardItem *experimentTypeItem;
    switch(experimentTypeItems.count()) {
        case 0:
            experimentTypeItem = new QStandardItem(descriptor->experimentType());
            m_DescriptorPoolModel.appendRow(experimentTypeItem);
            break;
        case 1:
            experimentTypeItem = experimentTypeItems.first();
            break;
        default:
            throw tr("More than one root object with the same name exists in the ModelDescriptor model<br/>"
                     "This is an unexpected behavior");
    }

    QStandardItem *descriptorItem = new QStandardItem();
    descriptorItem->setData(descriptor->name(), Qt::DisplayRole);
    descriptorItem->setData(descriptor->id().toString(), Qt::UserRole);
    experimentTypeItem->appendRow(descriptorItem);
}


/*! \brief Fetches default descriptors from the server and adds them to the descriptor pool.
    Phase II */
void ModelManager::fetchDescriptors()
{
    //TODO: Phase II.
    throw tr("Function not yet implemented.");


    /*! \note Don't forget to set each descriptor as a "default" descriptor, so it doesn't get exported, or modified by the
              user.  See ModelDescriptor::setDefault(bool) and ModelDescriptor::toXml(QList<ModelDescriptor *>) for more
              information. */

}

/*! \brief Exports the user's descriptors to a local XML file */
void ModelManager::exportDescriptors(const QString &filepath)
{
    QString filePath(filepath);
    if(filePath.isEmpty()) {
        Core::SettingManager::SettingManager *settingManager = Core::SettingManager::SettingManager::instance();
        filePath = QString("%1/ModelDescriptors.xml").arg(QApplication::instance()->applicationDirPath());
        filePath = settingManager->value("Plugins/OpenSpeedShop/ModelManager/DescriptorsFilePath", filePath).toString();
    }

    ModelDescriptor::toXml(filePath, m_DescriptorPool.values());
}

/*! \brief Creates a descriptor, stores it in the manager's descriptor pool, and returns it's UUID. Models are automatically
    updated. */
QUuid ModelManager::createDescriptor()
{
    ModelDescriptor *descriptor = new ModelDescriptor(this);
    m_DescriptorPool.insert(descriptor->id(), descriptor);
    insertDescriptorIntoModel(descriptor->id());
    return descriptor->id();
}

/*! \brief Removes the descriptor from the manager's descriptor pool. Models are automatically updated. */
void ModelManager::removeDescriptor(const QUuid &descriptorId)
{
    ModelDescriptor *descriptor = this->descriptor(descriptorId);
    QList<QStandardItem *> standardItems = m_DescriptorPoolModel.findItems(descriptor->name());

    foreach(QStandardItem *standardItem, standardItems) {
        if(standardItem->data(Qt::UserRole).toString() == descriptor->id().toString()) {
            m_DescriptorPoolModel.removeRow(standardItem->row(), standardItem->parent()->index());
            break;
        }
    }

    //TODO: remove any associated items in the m_ModelLookupTable
    //TODO: remove any associated items in the m_ModelPool

    m_DescriptorPool.remove(descriptorId);

    delete descriptor;
}

/*! \brief Creates and returns a widget to be used to interact with a single model descriptor.
    You are responsible for deleting this object. */
ModelDescriptorWidget *ModelManager::createDescriptorWidget(const QUuid &descriptorId, QWidget *parent)
{
    ModelDescriptorWidget *modelDescriptorWidget = new ModelDescriptorWidget(descriptor(descriptorId), parent);
    return modelDescriptorWidget;
}

/*! \brief Creates and returns a widget to be used to interact with all model descriptors in the pool.
    It has methods for filtering and sorting the descriptor pool.
    You are responsible for deleting this object. */
ModelDescriptorListWidget *ModelManager::createDescriptorListWidget(QWidget *parent)
{
    ModelDescriptorListWidget *descriptorListWidget = new ModelDescriptorListWidget(descriptorModel(), parent);
    return descriptorListWidget;
}

/*! \brief Returns a container for the MIME type data for the specified descriptor.
    Used during copy-and-paste and drag-and-drop operations. */
QMimeData ModelManager::descriptorMimeData(const QUuid &descriptorId)
{
    //TODO:
    throw tr("Function not yet implemented.");
}

/*! \brief Returns the descriptor object from the manager's descriptor pool.
    You don't own it; do not destroy it. */
ModelDescriptor *ModelManager::descriptor(QUuid descriptorId)
{
    return m_DescriptorPool.value(descriptorId, NULL);
}

/*! \brief Returns the model containing the list of descriptors in the manager's descriptor pool.
    There is only one model, you don't own it; do not destroy it. */
QAbstractItemModel *ModelManager::descriptorModel()
{
    return &m_DescriptorPoolModel;
}


/**********************************************************************************************************/


/*! \brief Fetches the model's data from the server, using the descriptor specified. */
QUuid ModelManager::fetchModel(const QUuid &descriptorId, const QUuid &experimentId)
{
    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();
    if(!serverAdapter) {
        throw tr("Cannot fetch model from server; not connected to server.");
    }

    // Fetch everything from the server, based on the descriptor
    ModelDescriptor *modelDescriptor = descriptor(descriptorId);
    DataModel *dataModel = serverAdapter->waitExperimentView(
                experimentId,
                modelDescriptor->modifiers(),
                modelDescriptor->metrics(),
                modelDescriptor->experimentType(),
                modelDescriptor->rowCount());
    m_ModelPool.insert(experimentId, dataModel);

    // Add everything to the look up table
    QHash<QUuid, QUuid> *experimentModels = new QHash<QUuid, QUuid>();
    experimentModels->insert(experimentId, dataModel->uid());
    m_ModelLookupTable.insert(descriptorId, experimentModels);

    return dataModel->uid();
}

/*! \brief Imports the model's data into the data pool.
    Phase II */
QUuid ModelManager::importModel(const QString &filepath)
{
    //TODO: Phase II.
    throw tr("Function not yet implemented.");
}

/*! \brief Exports the model's data to a local XML file.
    Phase II */
void ModelManager::ModelManager::exportModel(const QUuid &modelId, const QString &filepath)
{
    //TODO: Phase II.
    throw tr("Function not yet implemented.");
}

/*! \brief Removes the model's data from memory, requiring a re-fetch from the server if it is accessed again. */
void ModelManager::unloadModel(const QUuid &descriptorId, const QUuid &experimentId)
{
    QHash<QUuid, QUuid> *experimentModels = m_ModelLookupTable.value(descriptorId);
    if(!experimentModels || experimentModels->count() <= 0) {
        return;
    }

    QUuid modelId = experimentModels->value(experimentId);
    if(modelId.isNull()) {
        return;
    }

    DataModel *dataModel = m_ModelPool.value(modelId);
    if(!dataModel) {
        return;
    }

    m_ModelPool.remove(modelId);
    experimentModels->remove(experimentId);

    if(experimentModels->count() <= 0) {
        m_ModelLookupTable.remove(descriptorId, experimentModels);
    }
}

/*! \brief Returns a container for the MIME type data for the specified descriptor.
    Used during copy-and-paste and drag-and-drop operations. */
QMimeData ModelManager::modelMimeData(const QUuid &descriptorId, const QUuid &experimentId)
{
    //TODO:
    throw tr("Function not yet implemented.");
}

/*! \brief Returns the model from the cache. If it's not in the cache a fetch is automatically performed. */
QAbstractItemModel *ModelManager::model(const QUuid &descriptorId, const QUuid &experimentId)
{
    QUuid modelId;

    // Try to find it in the cache
    QHash<QUuid, QUuid> *experimentModels = m_ModelLookupTable.value(descriptorId);
    if(experimentModels && experimentModels->count() > 0) {
        modelId = experimentModels->value(experimentId);
    }

    // If the model wasn't cached, fetch it from the server
    if(modelId.isNull()) {
        modelId = fetchModel(descriptorId, experimentId);
    }

    return m_ModelPool.value(modelId);
}



} // namespace OpenSpeedShop
} // namespace Plugins
