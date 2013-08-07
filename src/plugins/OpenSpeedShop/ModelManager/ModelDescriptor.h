#ifndef PLUGINS_OPENSPEEDSHOP_MODELDESCRIPTOR_H
#define PLUGINS_OPENSPEEDSHOP_MODELDESCRIPTOR_H

#include <QObject>
#include <QUuid>
#include <QDomDocument>
#include <QStringList>

#include "ModelManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class MODELMANAGER_EXPORT ModelDescriptor : public QObject
{
    Q_OBJECT
public:
    explicit ModelDescriptor(QObject *parent = 0);

    QUuid id() const;
    void setId(const QString &uuid);
    void setId(const QUuid &uuid);

    QString name() const;
    void setName(const QString &name);

    QString experimentType() const;
    void setExperimentType(const QString &experimentType);

    quint64 rowCount() const;
    void setRowCount(const quint64 &rowCount);

    QStringList modifiers() const;
    void clearModifiers();
    void removeModifier(const QString &modifier);
    void insertModifier(const QString &modifier);

    QStringList metrics() const;
    void clearMetrics();
    void removeMetric(const QString &metric);
    void insertMetric(const QString &metric);

    bool isEmpty();

    bool isRemoving();
    void setRemoving(bool value = true);

    bool isDefault();
    void setDefault(bool value);

    static QList<ModelDescriptor *> fromXml(const QString &filePath, QObject *parent = NULL);
    static QList<ModelDescriptor *> fromXml(QDomDocument modelDescriptorDocument, QObject *parent = NULL);
    static ModelDescriptor *fromXml(const QDomElement &modelDescriptorElement, QObject *parent = NULL);
    static void toXml(const QString &filepath, QList<ModelDescriptor *> modelDescriptors);
    static QDomDocument toXml(QList<ModelDescriptor *> modelDescriptors);
    QDomElement toXml(QDomDocument document);

signals:
    void idChanged();
    void nameChanged();
    void experimentTypeChanged();
    void rowCountChanged();
    void modifiersChanged();
    void metricsChanged();
    void dataChanged();

protected:
    QUuid m_Id;
    QString m_Name;
    QString m_ExperimentType;
    quint64 m_RowCount;
    QStringList m_Modifiers;
    QStringList m_Metrics;
    bool m_Empty;
    bool m_Default;
    bool m_Removing;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // PLUGINS_OPENSPEEDSHOP_MODELDESCRIPTOR_H
