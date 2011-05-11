#include "ServerAdapter.h"

#include <QApplication>
#include "ConnectionManager.h"
#include "ServerCommand.h"

#include <QDebug>

namespace Plugins {
namespace OpenSpeedShop {

/*! \fn ServerAdapter::ServerAdapter()
    \brief
 */
ServerAdapter::ServerAdapter(QObject *parent) :
    QObject(parent)
{
}

/*! \fn ServerAdapter::rawCommand()
    \brief
 */
ServerCommand *ServerAdapter::rawCommand(QString command, QString type)
{
    ConnectionManager *connectionManager = ConnectionManager::instance();
    ServerCommand *serverCommand = new ServerCommand(command, type, this);
    connectionManager->sendCommand(serverCommand);
    return serverCommand;
}

/*! \fn ServerAdapter::rawServerCommand()
    \brief
 */
ServerCommand *ServerAdapter::rawServerCommand(QString command)
{
    return rawCommand(command, "Server");
}

/*! \fn ServerAdapter::rawOpenSpeedShopCommand()
    \brief
 */
ServerCommand *ServerAdapter::rawOpenSpeedShopCommand(QString command)
{
    return rawCommand(command, "OpenSpeedShopCLI");
}

/*! \fn ServerAdapter::waitCommand()
    \brief Simply waits for a command to get a response, and returns the response document.

    According to the Qt4 docs, the QDomElement is stored in memory "efficiently," meaning that we can copy very
    quickly. This is why we're not messing with pointers here.
 */
QDomElement ServerAdapter::waitCommand(ServerCommand *serverCommand)
{
    while(serverCommand->state() != ServerCommand::State_Response)
        QApplication::processEvents();

    QDomElement responseElement = serverCommand->response().firstChildElement("Response");
    if(responseElement.isNull()) throw QString("'Response' element doesn't exist, as expected.");

    return responseElement;
}

/*! \fn ServerAdapter::getString()
    \brief
 */
QString ServerAdapter::getString(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw QString("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw QString("'CommandObject' doesn't exist, as expected.");

    QDomElement child = commandObjectElement.firstChildElement("String");

    return child.attribute("value");
}

/*! \fn ServerAdapter::getStringList()
    \brief
 */
QStringList ServerAdapter::getStringList(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw QString("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw QString("'CommandObject' doesn't exist, as expected.");

    QStringList stringList;
    QDomElement child = commandObjectElement.firstChildElement("String");
    while(!child.isNull()) {
        // If the 'value' attribute hasn't been set, an empty string is automatically entered
        stringList.append(child.attribute("value"));
        child = child.nextSiblingElement("String");
    }

    return stringList;
}

/*! \fn ServerAdapter::getInt()
    \brief
 */
qint64 ServerAdapter::getInt(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw QString("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw QString("'CommandObject' doesn't exist, as expected.");

    QDomElement child = commandObjectElement.firstChildElement("Int");

    return child.attribute("value").toLongLong();
}

/*! \fn ServerAdapter::getIntList()
    \brief
 */
QList<qint64> ServerAdapter::getIntList(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw QString("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw QString("'CommandObject' doesn't exist, as expected.");

    QList<qint64> integerList;
    QDomElement child = commandObjectElement.firstChildElement("Int");
    while(!child.isNull()) {
        integerList.append(child.attribute("value", "-1").toLongLong());
        child = child.nextSiblingElement("String");
    }

    return integerList;
}


/*** BEGIN SocketServer commands *********************************************/

/*! \fn ServerAdapter::version()
    \brief
 */
ServerCommand *ServerAdapter::version()
{
    return rawServerCommand("version");
}

/*! \fn ServerAdapter::waitVersion()
    \brief
 */
QString ServerAdapter::waitVersion()
{
    ServerCommand *serverCommand = version();
    while(serverCommand->state() != ServerCommand::State_Response)
        QApplication::processEvents();

    QDomElement responseElement = serverCommand->response().firstChildElement("Response");
    if(responseElement.isNull()) throw QString("'Response' element doesn't exist, as expected.");

    QDomElement serverResponseElement = responseElement.firstChildElement("ServerResponse");
    if(serverResponseElement.isNull()) throw QString("'ServerResponse' doesn't exist, as expected.");

    if(!serverResponseElement.hasAttribute("version"))
        throw QString("'ServerResponse'' doesn't have 'version' attribute, as expected.");

    return serverResponseElement.attribute("version");
}

/*! \fn ServerAdapter::exit()
    \brief
 */
ServerCommand *ServerAdapter::exit()
{
    return rawServerCommand("exit");
}

/*! \fn ServerAdapter::waitExit()
    \brief
 */
void ServerAdapter::waitExit()
{
    ServerCommand *serverCommand = exit();
    while(serverCommand->state() != ServerCommand::State_Response)
        QApplication::processEvents();
}

/*** END SocketServer commands ***********************************************/


/*** BEGIN OpenSpeedShopCLI commands *****************************************/

/*! \fn ServerAdapter::ossHelp()
    \brief Good for testing purposes
 */
ServerCommand *ServerAdapter::ossHelp()
{
    return rawOpenSpeedShopCommand("help");
}

/*! \fn ServerAdapter::waitOssHelp()
    \brief Sychronous call to ossHelp(); good for testing purposes.
 */
QStringList ServerAdapter::waitOssHelp()
{
    ServerCommand *serverCommand = ossHelp();
    QDomElement responseElement = waitCommand(serverCommand);
    QStringList stringList = getStringList(responseElement);
    delete serverCommand;
    return stringList;
}

/*! \fn ServerAdapter::experimentTypes()
    \brief Lists experiment types available.
    \returns the asynchronous command object.
 */
ServerCommand *ServerAdapter::experimentTypes()
{
    QString command = QString("list -v exptypes -v all");
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentTypes()
    \brief Synchronously lists experiment types available.
    \returns A list of experiment types available.
 */
QStringList ServerAdapter::waitExperimentTypes()
{
    ServerCommand *serverCommand = experimentTypes();
    QDomElement responseElement = waitCommand(serverCommand);
    QStringList stringList = getStringList(responseElement);
    delete serverCommand;
    return stringList;
}

/*! \fn ServerAdapter::restore()
    \brief Opens an experiment from it's database file.
    \param filepath is the complete path to the database file.
    \returns the asynchronous command object.
 */
ServerCommand *ServerAdapter::restore(QString filepath)
{
    QString command = QString("expRestore -f %1").arg(filepath);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitRestore()
    \brief Synchronously opens an experiment from it's database file.
    \param filepath is the complete path to the database file.
    \returns The experiment identifier associated with the now open database file.
 */
qint64 ServerAdapter::waitRestore(QString filepath)
{
    ServerCommand *serverCommand = restore(filepath);
    QDomElement responseElement = waitCommand(serverCommand);
    return getInt(responseElement);
}

/*! \fn ServerAdapter::listOpenExperiments()
    \brief Lists the eperiment identifiers of currently open experiments.
    \returns the asynchronous command object.
 */
ServerCommand *ServerAdapter::listOpenExperiments()
{
    QString command = QString("list -v exp");
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitListOpenExperiments()
    \brief Synchronously lists the eperiment identifiers of currently open experiments.
    \returns The list of eperiment identifiers.
 */
QList<qint64> ServerAdapter::waitListOpenExperiments()
{
    ServerCommand *serverCommand = listOpenExperiments();
    QDomElement responseElement = waitCommand(serverCommand);
    return getIntList(responseElement);
}

/*! \fn ServerAdapter::experimentTypes()
    \brief Lists the experiment types associated with an experiment.
    \param expId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentTypes(qint64 expId)
{
    QString command = QString("list -v exp -x %1").arg(expId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentTypes()
    \brief Sycnronously lists the experiment types associated with an experiment.
    \param expId the experiment identifier.
    \returns a list of experiment type names associated with the experiment
 */
QStringList ServerAdapter::waitExperimentTypes(qint64 expId)
{
    ServerCommand *serverCommand = experimentTypes(expId);
    QDomElement responseElement = waitCommand(serverCommand);
    return getStringList(responseElement);
}

/*! \fn ServerAdapter::experimentDatabase()
    \brief Lists the database filepath associated with an experiment
    \param expId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentDatabase(qint64 expId)
{
    QString command = QString("list -v database -x %1").arg(expId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentDatabase()
    \brief Synchronously lists the database filepath associated with an experiment.
    \param expId the experiment identifier.
    \returns the database filepath associated with the experiment.
 */
QString ServerAdapter::waitExperimentDatabase(qint64 expId)
{
    ServerCommand *serverCommand = experimentDatabase(expId);
    QDomElement responseElement = waitCommand(serverCommand);
    return getString(responseElement);
}

/*! \fn ServerAdapter::experimentMetrics()
    \brief Lists the metrics associated with an experiment.
    \param expId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentMetrics(qint64 expId)
{
    QString command = QString("list -v metrics -x %1").arg(expId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentMetrics()
    \brief Synchronously lists the metrics associated with an experiment.
    \param expId the experiment identifier.
    \returns the list of metrics associated with the experiment.
 */
QStringList ServerAdapter::waitExperimentMetrics(qint64 expId)
{
    ServerCommand *serverCommand = experimentMetrics(expId);
    QDomElement responseElement = waitCommand(serverCommand);
    return getStringList(responseElement);
}

/*! \fn ServerAdapter::experimentRanks()
    \brief Lists the ranks associated with an experiment.
    \param expId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentRanks(qint64 expId)
{
    QString command = QString("list -v ranks -x %1").arg(expId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentRanks()
    \brief Synchronously lists the ranks associated with an experiment.
    \param expId the experiment identifier.
    \returns the list of ranks associated with the experiment.
 */
QList<qint64> ServerAdapter::waitExperimentRanks(qint64 expId)
{
    ServerCommand *serverCommand = experimentRanks(expId);
    QDomElement responseElement = waitCommand(serverCommand);
    return getIntList(responseElement);
}




ServerCommand *ServerAdapter::experimentView(qint64 expId, QString expType, int resolution)
{
    QString command = QString("expView -x %1 -v CallTrees,FullStack").arg(expId);
    if(!expType.isEmpty()) {
        command.append(QString(" %1%2").arg(expType).arg(resolution));
    }
    return rawOpenSpeedShopCommand(command);
}
void ServerAdapter::waitExperimentView(qint64 expId, QString expType, int resolution)
{
    ServerCommand *serverCommand = experimentView(expId, expType, resolution);
    QDomElement responseElement = waitCommand(serverCommand);

    qDebug() << serverCommand->response().toString();
//    return getStringList(responseElement);
}




/*** END OpenSpeedShopCLI commands *******************************************/


} // namespace OpenSpeedShop
} // namespace Plugins
