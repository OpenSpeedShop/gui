#include "ModelManager.h"

#include "ModelDescriptor.h"
#include "ModelDescriptorWidget.h"
#include "ModelListWidget.h"

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
    fromXml("ModelDescriptors.xml");
}

void ModelManager::fromXml(QString filePath)
{
    QDomDocument document = QDomDocument("ModelDescriptors");

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        throw tr("Could not open model description file: %1").arg(filePath);
    }

    if (!document.setContent(&file)) {
        file.close();
        throw tr("Could not use model description file after opening --possibly invalid text");
    }

    file.close();

    fromXml(document);
}

void ModelManager::fromXml(QDomDocument document)
{
    m_ModelDescriptors = ModelDescriptor::fromXml(document);
}

ModelListWidget *ModelManager::createModelListWidget(QString filter, QWidget *parent)
{
    ModelListWidget *modelListWidget = new ModelListWidget(parent);
    modelListWidget->setFilter(filter);
    modelListWidget->setModelDescriptors(m_ModelDescriptors);
    return modelListWidget;
}

} // namespace OpenSpeedShop
} // namespace Plugins
