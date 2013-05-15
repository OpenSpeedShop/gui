#ifndef SERVERCOMMAND_H
#define SERVERCOMMAND_H

#include <QObject>
#include <QDomDocument>
#include <QUuid>

#include "ConnectionManagerLibrary.h"

namespace Plugins {
namespace OpenSpeedShop {

class IConnection;

class CONNECTIONMANAGER_EXPORT ServerCommand : public QObject
{
    Q_OBJECT
public:
    enum States {
        State_Created,
        State_Sent,
        State_Response,
        State_Invalid
    };

    explicit ServerCommand(QString command, QString type = QString(), QObject *parent = 0);

    QDomDocument command();
    QString commandText();
    QString commandType();
    QString commandID();

    QDomDocument response();

    States state();

signals:
    void readyResponse();
    void stateChanged();

protected:
    explicit ServerCommand(QDomDocument command, QObject *parent = 0);
    void setResponse(QDomDocument response);
    void setState(States state);

    QDomDocument m_Command;
    QDomDocument m_Response;
    States m_State;
    QUuid m_id;

    friend class ConnectionManager;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // SERVERCOMMAND_H
