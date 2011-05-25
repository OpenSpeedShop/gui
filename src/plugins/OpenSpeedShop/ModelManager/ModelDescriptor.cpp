#include "ModelDescriptor.h"

#include <QFileInfo>

namespace Plugins {
namespace OpenSpeedShop {

ModelDescriptor::ModelDescriptor(QObject *parent) :
    QObject(parent),
    m_Id(QUuid::createUuid())
{
    m_RowCount = 100;
    m_Empty = true;
    m_Default = false;
}

QUuid ModelDescriptor::id() const
{
    return m_Id;
}

void ModelDescriptor::setId(const QString &uuid)
{
    m_Id = QUuid(uuid);

    m_Empty = false;
    emit idChanged();
    emit dataChanged();
}

void ModelDescriptor::setId(const QUuid &uuid)
{
    m_Id = QUuid(uuid);

    m_Empty = false;
    emit idChanged();
    emit dataChanged();
}

QString ModelDescriptor::name() const
{
    return m_Name;
}

void ModelDescriptor::setName(const QString &name)
{
    m_Name = name;

    m_Empty = false;
    emit nameChanged();
    emit dataChanged();
}

QString ModelDescriptor::experimentType() const
{
    return m_ExperimentType;
}

void ModelDescriptor::setExperimentType(const QString &experimentType)
{
    m_ExperimentType = experimentType;

    m_Empty = false;
    emit experimentTypeChanged();
    emit dataChanged();
}

quint64 ModelDescriptor::rowCount() const
{
    return m_RowCount;
}

void ModelDescriptor::setRowCount(const quint64 &rowCount)
{
    m_RowCount = rowCount;

    m_Empty = false;
    emit rowCountChanged();
    emit dataChanged();
}

QStringList ModelDescriptor::modifiers() const
{
    return m_Modifiers;
}

void ModelDescriptor::removeModifier(const QString &modifier)
{
    m_Modifiers.removeOne(modifier);

    m_Empty = false;
    emit modifiersChanged();
    emit dataChanged();
}

void ModelDescriptor::insertModifier(const QString &modifier)
{
    m_Modifiers.append(modifier);

    m_Empty = false;
    emit modifiersChanged();
    emit dataChanged();
}

QStringList ModelDescriptor::metrics() const
{
    return m_Metrics;
}

void ModelDescriptor::removeMetric(const QString &metric)
{
    m_Metrics.removeOne(metric);

    m_Empty = false;
    emit metricsChanged();
    emit dataChanged();
}

void ModelDescriptor::insertMetric(const QString &metric)
{
    m_Metrics.append(metric);

    m_Empty = false;
    emit metricsChanged();
    emit dataChanged();
}

bool ModelDescriptor::isEmpty()
{
    return m_Empty;
}

bool ModelDescriptor::isDefault()
{
    return m_Default;
}

void ModelDescriptor::setDefault(bool value)
{
    m_Default = value;
}


QList<ModelDescriptor *> ModelDescriptor::fromXml(const QString &filePath, QObject *parent)
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

    return fromXml(document, parent);
}

QList<ModelDescriptor *> ModelDescriptor::fromXml(QDomDocument modelDescriptorDocument, QObject *parent)
{
    QDomElement rootElement = modelDescriptorDocument.firstChildElement("ModelDescriptors");
    if(rootElement.isNull()) {
        throw tr("Document did not contain 'ModelDescriptors' element as its root");
    }

    QList<ModelDescriptor *> modelDescriptors;
    QDomElement modelDescriptorElement = rootElement.firstChildElement("ModelDescriptor");
    while(!modelDescriptorElement.isNull()) {
        modelDescriptors.append(fromXml(modelDescriptorElement));
        modelDescriptorElement = modelDescriptorElement.nextSiblingElement(modelDescriptorElement.tagName());
    }

    return modelDescriptors;
}

ModelDescriptor *ModelDescriptor::fromXml(const QDomElement &modelDescriptorElement, QObject *parent)
{
    ModelDescriptor *modelDescriptor = new ModelDescriptor(parent);

    modelDescriptor->setId(modelDescriptorElement.attribute("id"));
    modelDescriptor->setName(modelDescriptorElement.attribute("name"));
    modelDescriptor->setExperimentType(modelDescriptorElement.attribute("experimentType"));
    modelDescriptor->setRowCount(modelDescriptorElement.attribute("rowCount").toULongLong());

    QDomElement element = modelDescriptorElement.firstChildElement();
    while(!element.isNull()) {
        if(!element.tagName().compare("Modifier", Qt::CaseInsensitive)) {
            modelDescriptor->insertModifier(element.attribute("value"));
        } else if(!element.tagName().compare("Metric", Qt::CaseInsensitive)) {
            modelDescriptor->insertMetric(element.attribute("value"));
        }
        element = element.nextSiblingElement();
    }

    return modelDescriptor;
}

void ModelDescriptor::toXml(const QString &filepath, QList<ModelDescriptor *> modelDescriptors)
{
    QDomDocument document = ModelDescriptor::toXml(modelDescriptors);

    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly)) {
        QFileInfo fileInfo(filepath);
        throw tr("Failed to open model descriptor export file: %1").arg(fileInfo.absoluteFilePath());
    }

    QByteArray documentByteArray = document.toString(-1).toLatin1();
    qint64 writtenBytes = file.write(documentByteArray);
    if(writtenBytes < documentByteArray.count()) {
        QFileInfo fileInfo(filepath);
        throw tr("Failed to export entire model descriptor document to file: %1").arg(fileInfo.absoluteFilePath());
    }

    file.flush();
    file.close();
}

QDomDocument ModelDescriptor::toXml(QList<ModelDescriptor *> modelDescriptors)
{
    QDomDocument document("ModelDescriptors");
    QDomElement rootElement = document.createElement("ModelDescriptors");
    document.appendChild(rootElement);

    foreach(ModelDescriptor *modelDescriptor, modelDescriptors) {
        if(modelDescriptor->isDefault())
            continue;

        rootElement.appendChild(modelDescriptor->toXml(document));
    }

    return document;
}

QDomElement ModelDescriptor::toXml(QDomDocument document)
{
    QDomElement modelDescriptorElement = document.createElement("ModelDescriptor");

    modelDescriptorElement.setAttribute("id", id().toString());
    modelDescriptorElement.setAttribute("name", name());
    modelDescriptorElement.setAttribute("experimentType", experimentType());
    modelDescriptorElement.setAttribute("rowCount", rowCount());

    foreach(QString modifier, modifiers()) {
        QDomElement modifierElement = document.createElement("Modifier");
        modifierElement.setAttribute("value", modifier);
        modelDescriptorElement.appendChild(modifierElement);
    }

    foreach(QString metric, metrics()) {
        QDomElement metricElement = document.createElement("Metric");
        metricElement.setAttribute("value", metric);
        modelDescriptorElement.appendChild(metricElement);
    }

    return modelDescriptorElement;
}


} // namespace OpenSpeedShop
} // namespace Plugins
