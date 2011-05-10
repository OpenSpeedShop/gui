#include "ServerCommand.h"

#include "IConnection.h"

namespace Plugins {
namespace OpenSpeedShop {

ServerCommand::ServerCommand(QDomDocument command, QObject *parent) :
    QObject(parent)
{
    m_id = QUuid::createUuid();
    m_State = State_Created;

    m_Command = command;
    m_Command.firstChildElement("Command").setAttribute("id", m_id.toString());
}

ServerCommand::ServerCommand(QString command, QString type, QObject *parent) :
    QObject(parent)
{
    m_id = QUuid::createUuid();
    m_State = State_Created;

    m_Command = QDomDocument("Command");
    QDomElement element = m_Command.createElement("Command");
    element.setAttribute("id", m_id.toString());
    element.setAttribute("type", type);
    element.appendChild(m_Command.createTextNode(command));
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

QDomDocument ServerCommand::response()
{
    return m_Response;
}

void ServerCommand::setResponse(QDomDocument response)
{
    m_Response = response;
    setState(State_Response);
    emit readyResponse();
}

ServerCommand::States ServerCommand::state()
{
    return m_State;
}

void ServerCommand::setState(States state)
{
    m_State = state;
    emit stateChanged();
}

} // namespace OpenSpeedShop
} // namespace Plugins
