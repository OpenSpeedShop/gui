#ifndef SERVERADAPTER_H
#define SERVERADAPTER_H

#include <QObject>
#include <QDomElement>
#include <QUuid>
#include "ConnectionManagerLibrary.h"
#include "ModelManager/DataModel.h"

namespace Plugins {
namespace OpenSpeedShop {

class ConnectionManager;
class ServerCommand;

class CONNECTIONMANAGER_EXPORT ServerAdapter : public QObject
{
    Q_OBJECT
public:

    enum Modes {
        Mode_Offline,
        Mode_Online
    };

    struct Process {
        QString host;
        qint64 processId;
        qint64 threadId;
        qint64 rank;
        QString executable;
    };

    explicit ServerAdapter(QObject *parent = 0);

    ServerCommand *rawCommand(QString command, QString type);
    ServerCommand *rawServerCommand(QString command);
    ServerCommand *rawOpenSpeedShopCommand(QString command);

    QDomElement waitCommand(ServerCommand *serverCommand);

    /*** BEGIN SocketServer commands *********************************************/
    ServerCommand *version();
    QString waitVersion();

    ServerCommand *exit();
    void waitExit();
    /*** END SocketServer commands ***********************************************/

    /*** BEGIN OpenSpeedShopCLI commands *****************************************/
    ServerCommand *ossHelp();
    QStringList waitOssHelp();

    ServerCommand *experimentTypes();
    QStringList waitExperimentTypes();

    ServerCommand *restore(QString filepath);
    QUuid waitRestore(QString filepath);

    ServerCommand *listOpenExperiments();
    QList<qint64> waitListOpenExperiments();

    ServerCommand *experimentTypes(qint64 experimentId);
    QString waitExperimentTypes(qint64 experimentId);
    QString waitExperimentTypes(QUuid experimentUid);

    ServerCommand *experimentDatabase(qint64 experimentId);
    QString waitExperimentDatabase(qint64 experimentId);
    QString waitExperimentDatabase(QUuid experimentUid);

    ServerCommand *experimentMetrics(qint64 experimentId);
    QStringList waitExperimentMetrics(qint64 experimentId);
    QStringList waitExperimentMetrics(QUuid experimentUid);

    ServerCommand *experimentRanks(qint64 experimentId);
    QList<qint64> waitExperimentRanks(qint64 experimentId);
    QList<qint64> waitExperimentRanks(QUuid experimentUid);

    ServerCommand *experimentAppCommand(qint64 experimentId);
    QString waitExperimentAppCommand(qint64 experimentId);
    QString waitExperimentAppCommand(QUuid experimentUid);

    ServerCommand *experimentExecutable(qint64 experimentId);
    QString waitExperimentExecutable(qint64 experimentId);
    QString waitExperimentExecutable(QUuid experimentUid);

    ServerCommand *experimentHosts(qint64 experimentId);
    QStringList waitExperimentHosts(qint64 experimentId);
    QStringList waitExperimentHosts(QUuid experimentUid);

    ServerCommand *experimentPids(qint64 experimentId);
    QList<qint64> waitExperimentPids(qint64 experimentId);
    QList<qint64> waitExperimentPids(QUuid experimentUid);

    ServerCommand *experimentSourceFiles(qint64 experimentId);
    QStringList waitExperimentSourceFiles(qint64 experimentId);
    QStringList waitExperimentSourceFiles(QUuid experimentUid);

    ServerCommand *experimentObjectFiles(qint64 experimentId);
    QStringList waitExperimentObjectFiles(qint64 experimentId);
    QStringList waitExperimentObjectFiles(QUuid experimentUid);

    ServerCommand *experimentThreads(qint64 experimentId);
    QList<qint64> waitExperimentThreads(qint64 experimentId);
    QList<qint64> waitExperimentThreads(QUuid experimentUid);

    ServerCommand *experimentViews(QString experimentType);
    QStringList waitExperimentViews(QString experimentType);

    ServerCommand *experimentParameterValues(qint64 experimentId);
    QMap<QString, QVariant> waitExperimentParameterValues(qint64 experimentId);
    QMap<QString, QVariant> waitExperimentParameterValues(QUuid experimentUid);

    ServerCommand *experimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    DataModel *waitExperimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    DataModel *waitExperimentView(QUuid experimentUid, QString experimentType = QString(), int count = 1000);

    ServerCommand *experimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);
    DataModel *waitExperimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);
    DataModel *waitExperimentView(QUuid experimentUid, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);

    QList<Process> waitExperimentProcesses(qint64 experimentId);
    QList<Process> waitExperimentProcesses(QUuid experimentUid);

    QMap<QString,QString> waitExperimentTypeModifiers(QString experimentType);
    QMap<QString,QString> waitExperimentTypeMetrics(QString experimentType);

    /*** END OpenSpeedShopCLI commands *******************************************/

    int experimentId(QUuid experimentUid);
    QUuid experimentUid(qint64 experimentId);

protected:
    QString getString(QDomElement responseElement);
    QStringList getStringList(QDomElement responseElement);

    qint64 getInt(QDomElement responseElement);
    QList<qint64> getIntList(QDomElement responseElement);

    void loadMetrics();
    QDomDocument m_MetricsList;

    QHash<QUuid, int> m_ExperimentIds;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // SERVERADAPTER_H
