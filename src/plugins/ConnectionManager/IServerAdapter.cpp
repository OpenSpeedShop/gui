#include "IServerAdapter.h"

#include "ConnectionManager.h"
#include "ServerCommand.h"


namespace Plugins {
namespace ConnectionManager {

IServerAdapter::IServerAdapter(QObject *parent) :
    QObject(parent)
{
}

void IServerAdapter::rawCommand(QString command)
{



    ServerCommand *serverCommand = new ServerCommand(command, "OpenSpeedShopCLI", this);
    ConnectionManager *connectionManager = ConnectionManager::instance();


    connectionManager->sendCommand(serverCommand);


}


} // namespace ConnectionManager
} // namespace Plugins
