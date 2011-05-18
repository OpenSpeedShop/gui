#ifndef SERVERADAPTER_H
#define SERVERADAPTER_H

#include <QObject>
#include <QDomElement>
#include "ConnectionManagerLibrary.h"
#include "../ViewManager/DataModel.h"

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
    ServerCommand *experimentTypes();
    ServerCommand *restore(QString filepath);
    ServerCommand *listOpenExperiments();
    ServerCommand *experimentTypes(qint64 experimentId);
    ServerCommand *experimentDatabase(qint64 experimentId);
    ServerCommand *experimentMetrics(qint64 experimentId);
    ServerCommand *experimentRanks(qint64 experimentId);
    ServerCommand *experimentAppCommand(qint64 experimentId);
    ServerCommand *experimentExecutable(qint64 experimentId);
    ServerCommand *experimentHosts(qint64 experimentId);
    ServerCommand *experimentPids(qint64 experimentId);
    ServerCommand *experimentSourceFiles(qint64 experimentId);
    ServerCommand *experimentObjectFiles(qint64 experimentId);
    ServerCommand *experimentThreads(qint64 experimentId);
    ServerCommand *experimentViews(QString experimentType);
    ServerCommand *experimentParameterValues(qint64 experimentId);

    ServerCommand *experimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    ServerCommand *experimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);

    QStringList waitOssHelp();
    QStringList waitExperimentTypes();
    qint64 waitRestore(QString filepath);
    QList<qint64> waitListOpenExperiments();
    QString waitExperimentTypes(qint64 experimentId);
    QString waitExperimentDatabase(qint64 experimentId);
    QStringList waitExperimentMetrics(qint64 experimentId);
    QList<qint64> waitExperimentRanks(qint64 experimentId);
    QString waitExperimentAppCommand(qint64 experimentId);
    QString waitExperimentExecutable(qint64 experimentId);
    QStringList waitExperimentHosts(qint64 experimentId);
    QList<qint64> waitExperimentPids(qint64 experimentId);
    QStringList waitExperimentSourceFiles(qint64 experimentId);
    QStringList waitExperimentObjectFiles(qint64 experimentId);
    QList<qint64> waitExperimentThreads(qint64 experimentId);
    QStringList waitExperimentViews(QString experimentType);
    QMap<QString, QVariant> waitExperimentParameterValues(qint64 experimentId);

    DataModel *waitExperimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    DataModel *waitExperimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);

    QList<Process> waitExperimentProcesses(qint64 experimentId);

    QMap<QString,QString> waitExperimentTypeModifiers(QString experimentType);
    QMap<QString,QString> waitExperimentTypeMetrics(QString experimentType);

    /*** END OpenSpeedShopCLI commands *******************************************/


protected:
    QString getString(QDomElement responseElement);
    QStringList getStringList(QDomElement responseElement);

    qint64 getInt(QDomElement responseElement);
    QList<qint64> getIntList(QDomElement responseElement);

    void loadMetrics();
    QDomDocument m_MetricsList;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // SERVERADAPTER_H
