#ifndef SERVERCOMMAND_H
#define SERVERCOMMAND_H

#include <QObject>
#include <QDomDocument>
#include <QUuid>

namespace Plugins {
namespace ConnectionManager {

class IConnection;

enum ServerCommandStates {
    ServerCommandState_Created,
    ServerCommandState_Sent,
    ServerCommandState_Response,
    ServerCommandState_Invalid
};

class ServerCommand : public QObject
{
    Q_OBJECT
public:
    explicit ServerCommand(QString command, QString type = QString(), QObject *parent = 0);

    QDomDocument command();
    QString commandText();
    QString commandType();
    QString commandID();

    QDomDocument response();

    ServerCommandStates state();

signals:
    void readyResponse();
    void stateChanged();

protected:
    explicit ServerCommand(QDomDocument command, QObject *parent = 0);
    void setResponse(QDomDocument response);
    void setState(ServerCommandStates state);

    QDomDocument m_Command;
    QDomDocument m_Response;
    ServerCommandStates m_State;
    QUuid m_id;

    friend class ConnectionManager;
};

} // namespace ConnectionManager
} // namespace Plugins

#endif // SERVERCOMMAND_H
