#include "IServerAdapter.h"

#include "ConnectionManager.h"
#include "ServerCommand.h"


namespace Plugins {
namespace OpenSpeedShop {

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


} // namespace OpenSpeedShop
} // namespace Plugins
