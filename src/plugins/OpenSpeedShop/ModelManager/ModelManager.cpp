#include "ModelManager.h"

#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QFileInfo>

#include <ActionManager/ActionManager.h>
#include <PluginManager/PluginManager.h>
#include <SettingManager/SettingManager.h>

#include <ConnectionManager/ConnectionManager.h>
#include <ConnectionManager/IAdapter.h>

#include "DataModel.h"
#include "FilterDescriptor.h"
#include "ModelManagerDialog.h"
#include "ModelDescriptor.h"
#include "ModelDescriptorWidget.h"
#include "ModelDescriptorListWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

class ModelLookupRow {
public:
    ModelLookupRow(QUuid descriptorUid, QUuid experimentUid, QByteArray filterHash, QUuid modelUid) :
        m_DescriptorUid(descriptorUid), m_ExperimentUid(experimentUid), m_FilterHash(filterHash), m_ModelUid(modelUid) {}

    QUuid m_DescriptorUid;
    QUuid m_ExperimentUid;
    QByteArray m_FilterHash;
    QUuid m_ModelUid;
};


ModelManager &ModelManager::instance()
{
    static ModelManager m_Instance;
    return m_Instance;
}

ModelManager::ModelManager(QObject *parent) :
    QObject(parent)
{
    m_ModelManagerDialog = NULL;
}

ModelManager::~ModelManager()
{
    qDeleteAll(m_DescriptorPool);
    m_DescriptorPool.clear();
}

bool ModelManager::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {

        importDescriptors();

        /*** Register our menu structure ***/
        using namespace Core::ActionManager;
        ActionManager &actionManager = ActionManager::instance();

        Context *context = actionManager.createContext(); //TODO: Use OpenSpeedShopWidget's context as parent

        MenuPath menuPath("Tools");
        actionManager.createMenuPath(menuPath);

        m_ModelManagerDialog = new QAction(tr("Models Manager"), this);
        m_ModelManagerDialog->setToolTip(tr("Displays the Open|SpeedShop model manager dialog"));
        m_ModelManagerDialog->setIcon(QIcon(":/OpenSpeedShop/app.png"));
        m_ModelManagerDialog->setIconVisibleInMenu(true);
        m_ModelManagerDialog->setVisible(false);
        m_ModelManagerDialog->setProperty("oss_menuitem", QVariant(1));
        connect(m_ModelManagerDialog, SIGNAL(triggered()), this, SLOT(modelManagerDialog()));

        actionManager.registerAction(context, menuPath, m_ModelManagerDialog);



        using namespace Core::PluginManager;
        PluginManager &pluginManager = PluginManager::instance();
        pluginManager.addObject(this);

    } catch(...) {
        return false;
    }

    return true;
}

void ModelManager::shutdown()
{
    try {
        exportDescriptors();
    } catch(QString err) {
        qCritical() << tr("Error occured while shutting down ModelManager: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Error occured while shutting down ModelManager");
    }
}

/**********************************************************************************************************/

/*! \brief Loads the user's descriptors from a local XML file and adds them to the descriptor pool. */
void ModelManager::importDescriptors(const QString &filepath)
{
    QString filePath(filepath);
    if(filePath.isEmpty()) {
        //TODO: Create a setting for this in the setting console
        Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();

        //FIXME: This should not be hard coded here
#ifdef Q_OS_WIN
        filePath = QString("%1/oss/ModelDescriptors.xml").arg(QApplication::instance()->applicationDirPath());
#else
        filePath = QString("%1/../share/openspeedshop/gui/ModelDescriptors.xml").arg(QApplication::instance()->applicationDirPath());
#endif

        filePath = settingManager.value("Plugins/OpenSpeedShop/ModelManager/DescriptorsFilePath", filePath).toString();
    }

    if(!QFile::exists(filePath)) {
        QFileInfo fileInfo(filePath);
        throw tr("The ModelManager could not load the ModelDescriptor file: %1<br/>"
                       "This file path can be changed in the settings").arg(fileInfo.absoluteFilePath());
    }

    QList<ModelDescriptor *> modelDescriptors = ModelDescriptor::fromXml(filePath, this);

    foreach(ModelDescriptor *modelDescriptor, modelDescriptors) {
        if(m_DescriptorPool.keys().contains(modelDescriptor->id())) {
            throw tr("Multiple model descriptors with the same unique identifier have been found. Check the descriptor source!");
        }

        m_DescriptorPool.insert(modelDescriptor->id(), modelDescriptor);
        insertDescriptorIntoModel(modelDescriptor->id());
    }

    m_DescriptorPoolModel.setHeaderData(0, Qt::Horizontal, tr("Model"), Qt::DisplayRole);
}

/*! \brief Helper function that adds a descriptor from the descriptor pool into the standard model. */
void ModelManager::insertDescriptorIntoModel(QUuid descriptorUid)
{
    //FIXME: This is horribly inefficient.

    ModelDescriptor *descriptor = this->descriptor(descriptorUid);

    QStandardItem *descriptorItem = new QStandardItem();
    descriptorItem->setData(descriptor->id().toString(), Qt::UserRole);

    // Associate the descriptorItem for easier/quicker lookup
    m_DescriptorToItem.insert(descriptor->id(), descriptorItem);

    descriptorNameChanged(descriptor);
    connect(descriptor, SIGNAL(nameChanged()), this, SLOT(descriptorNameChanged()));

    descriptorTypeChanged(descriptor);
    connect(descriptor, SIGNAL(experimentTypeChanged()), this, SLOT(descriptorTypeChanged()));
}

void ModelManager::descriptorNameChanged(ModelDescriptor *descriptor)
{
    if(!descriptor) {
        descriptor = qobject_cast<ModelDescriptor *>(QObject::sender());
    }

    QStandardItem *descriptorItem = m_DescriptorToItem.value(descriptor->id());

    descriptorItem->setData(descriptor->name(), Qt::DisplayRole);
}

void ModelManager::descriptorTypeChanged(ModelDescriptor *descriptor)
{
    if(!descriptor) {
        descriptor = qobject_cast<ModelDescriptor *>(QObject::sender());
    }

    QStandardItem *descriptorItem = m_DescriptorToItem.value(descriptor->id());
    QStandardItem *parentItem = descriptorItem->parent();
    if(parentItem) {
        if(!parentItem->data(Qt::DisplayRole).toString().compare(descriptor->experimentType())) {
            return;
        }

        parentItem->takeRow(descriptorItem->row());

        if(parentItem->rowCount() <= 0) {
            parentItem->model()->removeRow(parentItem->row());
        }
    }

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
            experimentTypeItem->setSelectable(false);
            m_DescriptorPoolModel.appendRow(experimentTypeItem);
            break;
        case 1:
            experimentTypeItem = experimentTypeItems.first();
            break;
        default:
            throw tr("More than one root object with the same name exists in the ModelDescriptor model<br/>"
                     "This is an unexpected behavior");
    }

    experimentTypeItem->appendRow(descriptorItem);
}

void ModelManager::modelManagerDialog()
{
    try {

        ModelManagerDialog models;
        models.exec();

    } catch(QString err) {
        qCritical() << tr("Failed to open model manager dialog: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to open model manager dialog.");
    }
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
        Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();

        //FIXME: This should not be hard coded here
#ifdef Q_OS_WIN
        filePath = QString("%1/oss/ModelDescriptors.xml").arg(QApplication::instance()->applicationDirPath());
#else
        filePath = QString("%1/../share/openspeedshop/gui/ModelDescriptors.xml").arg(QApplication::instance()->applicationDirPath());
#endif

        filePath = settingManager.value("Plugins/OpenSpeedShop/ModelManager/DescriptorsFilePath", filePath).toString();
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
void ModelManager::removeDescriptor(const QUuid &descriptorUid)
{
    ModelDescriptor *descriptor = this->descriptor(descriptorUid);
    if(!descriptor) { return; }
    descriptor->setRemoving();

    QList<QStandardItem *> standardItems = m_DescriptorPoolModel.findItems(descriptor->name());

    foreach(QStandardItem *standardItem, standardItems) {
        if(standardItem->data(Qt::UserRole).toString() == descriptor->id().toString()) {
            m_DescriptorPoolModel.removeRow(standardItem->row(), standardItem->parent()->index());
            break;
        }
    }

    //TODO: remove any associated items in the m_ModelLookupTable
    //TODO: remove any associated items in the m_ModelPool

    // Remove associated objects from the model, and the model item cache
    QStandardItem *descriptorItem = m_DescriptorToItem.value(descriptorUid);
    if(descriptorItem) {
        descriptorItem->parent()->removeRow(descriptorItem->row()); //This deletes descriptorItem
    }

    m_DescriptorToItem.remove(descriptorUid);
    m_DescriptorPool.remove(descriptorUid);

    descriptor->deleteLater();
}

/*! \brief Creates and returns a widget to be used to interact with a single model descriptor.
    You are responsible for deleting this object. */
ModelDescriptorWidget *ModelManager::createDescriptorWidget(const QUuid &descriptorUid, QWidget *parent)
{
    ModelDescriptor *descriptor = this->descriptor(descriptorUid);
    if(!descriptor) {
        return NULL;
    }

    ModelDescriptorWidget *modelDescriptorWidget = new ModelDescriptorWidget(descriptor, parent);
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
QMimeData ModelManager::descriptorMimeData(const QUuid &descriptorUid)
{
    Q_UNUSED(descriptorUid)

    //TODO:
    throw tr("Function not yet implemented.");
}

/*! \brief Returns the descriptor object from the manager's descriptor pool.
    You don't own it; do not destroy it. */
ModelDescriptor *ModelManager::descriptor(QUuid descriptorUid)
{
    return m_DescriptorPool.value(descriptorUid, NULL);
}

/*! \brief Returns the model containing the list of descriptors in the manager's descriptor pool.
    There is only one model, you don't own it; do not destroy it. */
QAbstractItemModel *ModelManager::descriptorModel()
{
    return &m_DescriptorPoolModel;
}


/**********************************************************************************************************/


/*! \brief Fetches the model's data from the server, using the descriptor specified. */
QUuid ModelManager::fetchModel(const QUuid &descriptorUid, const QUuid &experimentUid, const FilterDescriptor *filterDescriptor)
{
    ConnectionManager &connectionManager = ConnectionManager::instance();
    if(!connectionManager.isConnected()) {
        if(!connectionManager.askServerConnect()) {
            throw tr("Server not connected");
        }
    }

    IAdapter *adapter = connectionManager.currentAdapter();
    if(!adapter) throw tr("Server not connected");


#ifdef MODELMANAGER_DEBUG
        qDebug() << __FILE__ << ":" << __LINE__ << "Fetching model from server";
#endif

    // Fetch everything from the server, based on the descriptor
    ModelDescriptor *modelDescriptor = descriptor(descriptorUid);
    QAbstractItemModel *model = adapter->waitExperimentView(
                experimentUid,
                modelDescriptor->modifiers(),
                modelDescriptor->metrics(),
                filterDescriptor->filters(),
                modelDescriptor->rowCount());

    DataModel *dataModel = qobject_cast<DataModel *>(model);
    if(!dataModel) {
        throw tr("Odd data type returned from server adapter; expected DataModel QObject from IAdapter::waitExperimentView()");
    }
    m_ModelPool.insert(dataModel->uid(), dataModel);

#ifdef MODELMANAGER_DEBUG
    qDebug() << __FILE__ << ":" << __LINE__ << "dataModel:" << (dataModel? "!NULL": "NULL");
    qDebug() << __FILE__ << ":" << __LINE__ << "dataModelUid" << dataModel->uid();
#endif

    // Add everything to the look up table
    m_ModelLookupTable.append(new ModelLookupRow(descriptorUid, experimentUid, filterDescriptor->getHash(), dataModel->uid()));

    return dataModel->uid();
}

/*! \brief Imports the model's data into the data pool.
    Phase II */
QUuid ModelManager::importModel(const QString &filepath)
{
    Q_UNUSED(filepath)

    //TODO: Phase II.
    throw tr("Function not yet implemented.");
}

/*! \brief Exports the model's data to a local XML file.
    Phase II */
void ModelManager::ModelManager::exportModel(const QUuid &modelUid, const QString &filepath)
{
    Q_UNUSED(modelUid)
    Q_UNUSED(filepath)

    //TODO: Phase II.
    throw tr("Function not yet implemented.");
}

/*! \brief Removes the model's data from memory, requiring a re-fetch from the server if it is accessed again. */
void ModelManager::unloadModel(const QUuid &descriptorUid, const QUuid &experimentUid, const FilterDescriptor *filterDescriptor)
{
    ModelLookupRow *modelLookupRow = NULL;
    foreach(ModelLookupRow *row, m_ModelLookupTable) {  // Not the fastest lookup method, but we're not expecting a huge list
        if(     row->m_DescriptorUid == descriptorUid &&
                row->m_ExperimentUid == experimentUid &&
                row->m_FilterHash == filterDescriptor->getHash()) {
            modelLookupRow = row;
        }
    }

    if(!modelLookupRow || modelLookupRow->m_ModelUid.isNull()) {  // isn't there anyway, just return
        return;
    }

    DataModel *model = m_ModelPool.value(modelLookupRow->m_ModelUid);
    if(!model) {
        m_ModelPool.remove(modelLookupRow->m_ModelUid);
        model->deleteLater();
    }

    m_ModelLookupTable.removeAll(modelLookupRow);
}

/*! \brief Returns a container for the MIME type data for the specified descriptor.
    Used during copy-and-paste and drag-and-drop operations. */
QMimeData ModelManager::modelMimeData(const QUuid &descriptorUid, const QUuid &experimentUid)
{
    Q_UNUSED(descriptorUid)
    Q_UNUSED(experimentUid)

    //TODO:
    throw tr("Function not yet implemented.");
}

/*! \brief Returns the model from the cache. If it's not in the cache a fetch is automatically performed. */
QAbstractItemModel *ModelManager::model(const QUuid &descriptorUid, const QUuid &experimentUid, const FilterDescriptor &filterDescriptor)
{
    // Try to find it in the cache
    ModelLookupRow *modelLookupRow = NULL;
    foreach(ModelLookupRow *row, m_ModelLookupTable) {  // Not the fastest lookup method, but we're not expecting a huge list
        if(     row->m_DescriptorUid == descriptorUid &&
                row->m_ExperimentUid == experimentUid &&
                row->m_FilterHash == filterDescriptor.getHash()) {
            modelLookupRow = row;
        }
    }

    // If the model wasn't cached, fetch it from the server
    QUuid modelUid;
    if(!modelLookupRow || modelLookupRow->m_ModelUid.isNull()) {
        modelUid = fetchModel(descriptorUid, experimentUid, &filterDescriptor);
    } else {
        modelUid = modelLookupRow->m_ModelUid;
    }

#ifdef MODELMANAGER_DEBUG
    qDebug() << __FILE__ << ":" << __LINE__ << "ModelUid:" << modelUid;
#endif

    return m_ModelPool.value(modelUid, NULL);
}



} // namespace OpenSpeedShop
} // namespace Plugins
