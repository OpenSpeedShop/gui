#include "ServerCommand.h"

#include "IConnection.h"

namespace Plugins {
namespace OpenSpeedShop {

ServerCommand::ServerCommand(QDomDocument command, QObject *parent) :
    QObject(parent)
{
    m_id = QUuid::createUuid();
    m_State = ServerCommandState_Created;

    m_Command = command;
    m_Command.firstChildElement("Command").setAttribute("id", m_id.toString());
}

ServerCommand::ServerCommand(QString command, QString type, QObject *parent) :
    QObject(parent)
{
    m_id = QUuid::createUuid();
    m_State = ServerCommandState_Created;

    m_Command = QDomDocument("Command");
    QDomElement element = m_Command.createElement("Command");
    element.setAttribute("id", m_id.toString());
    element.setAttribute("type", type);
    element.setNodeValue(command);
    m_Command.appendChild(element);
}

QDomDocument ServerCommand::command()
{
    return m_Command;
}

QString ServerCommand::commandText()
{
    return command().firstChildElement("Command").text();
}

QString ServerCommand::commandType()
{
    return command().firstChildElement("Command").attribute("type");
}

QString ServerCommand::commandID()
{
    return command().firstChildElement("Command").attribute("id");
}

void ServerCommand::setResponse(QDomDocument response)
{
    m_Response = response;
    setState(ServerCommandState_Response);
    emit readyResponse();
}

void ServerCommand::setState(ServerCommandStates state)
{
    m_State = state;
    emit stateChanged();
}

} // namespace OpenSpeedShop
} // namespace Plugins
