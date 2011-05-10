#ifndef SERVERADAPTER_H
#define SERVERADAPTER_H

#include <QObject>
#include <QDomElement>
#include "ConnectionManagerLibrary.h"

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
    ServerCommand *experimentTypes(qint64 expId);
    ServerCommand *experimentDatabase(qint64 expId);
    ServerCommand *experimentMetrics(qint64 expId);
    ServerCommand *experimentRanks(qint64 expId);
    ServerCommand *experimentView(qint64 expId, QString expType = QString(), int resolution = 100);

    QStringList waitOssHelp();
    QStringList waitExperimentTypes();
    qint64 waitRestore(QString filepath);
    QList<qint64> waitListOpenExperiments();
    QStringList waitExperimentTypes(qint64 expId);
    QString waitExperimentDatabase(qint64 expId);
    QStringList waitExperimentMetrics(qint64 expId);
    QList<qint64> waitExperimentRanks(qint64 expId);
    void waitExperimentView(qint64 expId, QString expType = QString(), int resolution = 100);
    /*** END OpenSpeedShopCLI commands *******************************************/


protected:
    QString getString(QDomElement responseElement);
    QStringList getStringList(QDomElement responseElement);

    qint64 getInt(QDomElement responseElement);
    QList<qint64> getIntList(QDomElement responseElement);

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // SERVERADAPTER_H
