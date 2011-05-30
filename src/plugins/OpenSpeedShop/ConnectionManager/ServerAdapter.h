#ifndef SERVERADAPTER_H
#define SERVERADAPTER_H

#include <QDomElement>
#include "ConnectionManager/IAdapter.h"
#include "ConnectionManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class ConnectionManager;
class ServerCommand;

class CONNECTIONMANAGER_EXPORT ServerAdapter : public IAdapter
{
    Q_OBJECT
public:
    explicit ServerAdapter(QObject *parent = 0);

    bool isCompatible(QString serverVersion);

    QString waitVersion();
    void waitExit();

    QStringList waitOssHelp();
    QStringList waitExperimentTypes();
    QUuid waitRestore(QString filepath);
    QList<qint64> waitListOpenExperiments();
    QString waitExperimentType(QUuid experimentUid);
    QString waitExperimentDatabase(QUuid experimentUid);
    QStringList waitExperimentMetrics(QUuid experimentUid);
    QList<qint64> waitExperimentRanks(QUuid experimentUid);
    QString waitExperimentAppCommand(QUuid experimentUid);
    QString waitExperimentExecutable(QUuid experimentUid);
    QStringList waitExperimentHosts(QUuid experimentUid);
    QList<qint64> waitExperimentPids(QUuid experimentUid);
    QStringList waitExperimentSourceFiles(QUuid experimentUid);
    QStringList waitExperimentObjectFiles(QUuid experimentUid);
    QList<qint64> waitExperimentThreads(QUuid experimentUid);
    QStringList waitExperimentViews(QString experimentType);
    QMap<QString, QVariant> waitExperimentParameterValues(QUuid experimentUid);
    QAbstractItemModel *waitExperimentView(QUuid experimentUid, int count = 1000);
    QAbstractItemModel *waitExperimentView(QUuid experimentUid, QStringList modifiers, QStringList metrics, int count = 1000);
    QList<Process> waitExperimentProcesses(QUuid experimentUid);
    QMap<QString,QString> waitExperimentTypeModifiers(QString experimentType);
    QMap<QString,QString> waitExperimentTypeMetrics(QString experimentType);

protected:
    ServerCommand *version();
    ServerCommand *exit();

    ServerCommand *ossHelp();
    ServerCommand *experimentTypes();
    ServerCommand *restore(QString filepath);
    ServerCommand *listOpenExperiments();
    ServerCommand *experimentType(qint64 experimentId);
    QString waitExperimentType(qint64 experimentId);
    ServerCommand *experimentDatabase(qint64 experimentId);
    QString waitExperimentDatabase(qint64 experimentId);
    ServerCommand *experimentMetrics(qint64 experimentId);
    QStringList waitExperimentMetrics(qint64 experimentId);
    ServerCommand *experimentRanks(qint64 experimentId);
    QList<qint64> waitExperimentRanks(qint64 experimentId);
    ServerCommand *experimentAppCommand(qint64 experimentId);
    QString waitExperimentAppCommand(qint64 experimentId);
    ServerCommand *experimentExecutable(qint64 experimentId);
    QString waitExperimentExecutable(qint64 experimentId);
    ServerCommand *experimentHosts(qint64 experimentId);
    QStringList waitExperimentHosts(qint64 experimentId);
    ServerCommand *experimentPids(qint64 experimentId);
    QList<qint64> waitExperimentPids(qint64 experimentId);
    ServerCommand *experimentSourceFiles(qint64 experimentId);
    QStringList waitExperimentSourceFiles(qint64 experimentId);
    ServerCommand *experimentObjectFiles(qint64 experimentId);
    QStringList waitExperimentObjectFiles(qint64 experimentId);
    ServerCommand *experimentThreads(qint64 experimentId);
    QList<qint64> waitExperimentThreads(qint64 experimentId);
    ServerCommand *experimentViews(QString experimentType);
    ServerCommand *experimentParameterValues(qint64 experimentId);
    QMap<QString, QVariant> waitExperimentParameterValues(qint64 experimentId);
    ServerCommand *experimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    QAbstractItemModel *waitExperimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    ServerCommand *experimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);
    QAbstractItemModel *waitExperimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);
    QList<Process> waitExperimentProcesses(qint64 experimentId);

    ServerCommand *rawCommand(QString command, QString type);
    ServerCommand *rawServerCommand(QString command);
    ServerCommand *rawOpenSpeedShopCommand(QString command);
    QDomElement waitCommand(ServerCommand *serverCommand);
    QString getString(QDomElement responseElement);
    QStringList getStringList(QDomElement responseElement);
    qint64 getInt(QDomElement responseElement);
    QList<qint64> getIntList(QDomElement responseElement);

    int experimentId(QUuid experimentUid);
    QUuid experimentUid(qint64 experimentId);

    void loadMetrics();
    QDomDocument m_MetricsList;

    QHash<QUuid, int> m_ExperimentIds;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // SERVERADAPTER_H
