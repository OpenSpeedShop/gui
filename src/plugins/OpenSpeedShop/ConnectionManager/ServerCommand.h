#ifndef PLUGINS_OPENSPEEDSHOP_SERVERCOMMAND_H
#define PLUGINS_OPENSPEEDSHOP_SERVERCOMMAND_H

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

    explicit ServerCommand(const QString &command, const QString &type = QString(), QObject *parent = 0);

    QDomDocument command() const;
    QString commandText() const;
    QString commandType() const;
    QString commandID() const;
    QUuid commandUid() const;

    QDomDocument response() const;

    States state() const;

signals:
    void readyResponse();
    void stateChanged();

protected:
    explicit ServerCommand(const QDomDocument &command, QObject *parent = 0);
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

#endif // PLUGINS_OPENSPEEDSHOP_SERVERCOMMAND_H
