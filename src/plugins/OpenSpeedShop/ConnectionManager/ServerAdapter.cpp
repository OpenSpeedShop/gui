#include "ServerAdapter.h"

#include <QFile>
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
    if(responseElement.isNull()) throw tr("'Response' element doesn't exist, as expected.");

    return responseElement;
}

/*! \fn ServerAdapter::getString()
    \brief
 */
QString ServerAdapter::getString(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw tr("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw tr("'CommandObject' doesn't exist, as expected.");

    QDomElement child = commandObjectElement.firstChildElement("String");

    return child.attribute("value");
}

/*! \fn ServerAdapter::getStringList()
    \brief
 */
QStringList ServerAdapter::getStringList(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw tr("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw tr("'CommandObject' doesn't exist, as expected.");

    QStringList stringList;
    QDomElement child = commandObjectElement.firstChildElement("String");
    while(!child.isNull()) {
        // If the 'value' attribute hasn't been set, an empty string is automatically entered
        stringList.append(child.attribute("value"));
        child = child.nextSiblingElement(child.tagName());
    }

    return stringList;
}

/*! \fn ServerAdapter::getInt()
    \brief
 */
qint64 ServerAdapter::getInt(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw tr("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw tr("'CommandObject' doesn't exist, as expected.");

    QDomElement child = commandObjectElement.firstChildElement("Int");

    return child.attribute("value").toLongLong();
}

/*! \fn ServerAdapter::getIntList()
    \brief
 */
QList<qint64> ServerAdapter::getIntList(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw tr("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw tr("'CommandObject' doesn't exist, as expected.");

    QList<qint64> integerList;
    QDomElement child = commandObjectElement.firstChildElement("Int");
    while(!child.isNull()) {
        integerList.append(child.attribute("value", "-1").toLongLong());
        child = child.nextSiblingElement(child.tagName());
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
    if(responseElement.isNull()) throw tr("'Response' element doesn't exist, as expected.");

    QDomElement serverResponseElement = responseElement.firstChildElement("ServerResponse");
    if(serverResponseElement.isNull()) throw tr("'ServerResponse' doesn't exist, as expected.");

    if(!serverResponseElement.hasAttribute("version"))
        throw tr("'ServerResponse'' doesn't have 'version' attribute, as expected.");

    QString version = serverResponseElement.attribute("version");
    delete serverCommand;
    return version;
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
    delete serverCommand;
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
    QString command = QString("list -v expTypes -v all");
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
    delete serverCommand;
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
    delete serverCommand;
    return getIntList(responseElement);
}

/*! \fn ServerAdapter::experimentTypes()
    \brief Lists the experiment types associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentTypes(qint64 experimentId)
{
    QString command = QString("list -v expTypes -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentTypes()
    \brief Sycnronously lists the experiment types associated with an experiment.
    \param experimentId the experiment identifier.
    \returns a list of experiment type names associated with the experiment
 */
QString ServerAdapter::waitExperimentTypes(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentTypes(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getString(responseElement);
}

/*! \fn ServerAdapter::experimentDatabase()
    \brief Lists the database filepath associated with an experiment
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentDatabase(qint64 experimentId)
{
    QString command = QString("list -v database -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentDatabase()
    \brief Synchronously lists the database filepath associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the database filepath associated with the experiment.
 */
QString ServerAdapter::waitExperimentDatabase(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentDatabase(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getString(responseElement);
}

/*! \fn ServerAdapter::experimentMetrics()
    \brief Lists the metrics associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentMetrics(qint64 experimentId)
{
    QString command = QString("list -v metrics -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentMetrics()
    \brief Synchronously lists the metrics associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the list of metrics associated with the experiment.
 */
QStringList ServerAdapter::waitExperimentMetrics(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentMetrics(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getStringList(responseElement);
}

/*! \fn ServerAdapter::experimentRanks()
    \brief Lists the ranks associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentRanks(qint64 experimentId)
{
    QString command = QString("list -v ranks -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentRanks()
    \brief Synchronously lists the ranks associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the list of ranks associated with the experiment.
 */
QList<qint64> ServerAdapter::waitExperimentRanks(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentRanks(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getIntList(responseElement);
}


ServerCommand *ServerAdapter::experimentAppCommand(qint64 experimentId)
{
    QString command = QString("list -v appcommand -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QString ServerAdapter::waitExperimentAppCommand(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentAppCommand(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getString(responseElement);
}

ServerCommand *ServerAdapter::experimentExecutable(qint64 experimentId)
{
    QString command = QString("list -v executable -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QString ServerAdapter::waitExperimentExecutable(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentExecutable(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getString(responseElement);
}

ServerCommand *ServerAdapter::experimentHosts(qint64 experimentId)
{
    QString command = QString("list -v hosts -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QStringList ServerAdapter::waitExperimentHosts(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentHosts(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getStringList(responseElement);
}

ServerCommand *ServerAdapter::experimentPids(qint64 experimentId)
{
    QString command = QString("list -v pids -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QList<qint64> ServerAdapter::waitExperimentPids(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentPids(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getIntList(responseElement);
}


ServerCommand *ServerAdapter::experimentSourceFiles(qint64 experimentId)
{
    QString command = QString("list -v src -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QStringList ServerAdapter::waitExperimentSourceFiles(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentSourceFiles(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getStringList(responseElement);
}

ServerCommand *ServerAdapter::experimentObjectFiles(qint64 experimentId)
{
    QString command = QString("list -v src -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QStringList ServerAdapter::waitExperimentObjectFiles(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentObjectFiles(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getStringList(responseElement);
}

ServerCommand *ServerAdapter::experimentThreads(qint64 experimentId)
{
    QString command = QString("list -v threads -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QList<qint64> ServerAdapter::waitExperimentThreads(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentThreads(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getIntList(responseElement);
}

ServerCommand *ServerAdapter::experimentViews(QString experimentType)
{
    QString command = QString("list -v views %1").arg(experimentType);
    return rawOpenSpeedShopCommand(command);
}
QStringList ServerAdapter::waitExperimentViews(QString experimentType)
{
    ServerCommand *serverCommand = experimentViews(experimentType);
    QDomElement responseElement = waitCommand(serverCommand);
    delete serverCommand;
    return getStringList(responseElement);
}

ServerCommand *ServerAdapter::experimentParameterValues(qint64 experimentId)
{
    QString command = QString("list -v paramvalues -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QMap<QString, QVariant> ServerAdapter::waitExperimentParameterValues(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentParameterValues(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw tr("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw tr("'CommandObject' doesn't exist, as expected.");

    // Iterate over the list of parameters and parse it into a QMap for the returned value
    QRegExp regex("[\\s=]*$");      // So we can remove the equal sign and spaces at the end
    QMap<QString, QVariant> parameters;
    QDomElement row = commandObjectElement.firstChildElement("Columns");
    while(!row.isNull()) {
        // Clean up the parameter name
        QDomElement nameElement = row.firstChildElement();
        QString nameString = nameElement.attribute("value");
        nameString = nameString.remove(regex);

        // Get the value string and value type
        QDomElement valueElement = nameElement.nextSiblingElement();
        QVariant value;
        bool okay = false;
        QString valueType = valueElement.tagName();
        QString valueString = valueElement.attribute("value");

        // If we know how to convert the type, attempt it
        if(!valueType.compare("Duration", Qt::CaseInsensitive) ||
           !valueType.compare("Float", Qt::CaseInsensitive)) {
            value = valueString.toDouble(&okay);

        } else if(!valueType.compare("Int", Qt::CaseInsensitive)) {
            value = valueString.toLongLong(&okay);

        } else if(!valueType.compare("Interval", Qt::CaseInsensitive) ||
                  !valueType.compare("Uint", Qt::CaseInsensitive)) {
            value = valueString.toULongLong(&okay);
        }

        // If all else fails, just use the string value
        if(!okay) value = valueString;

        // Save the key-value pair and move on to the next one
        parameters[nameString] = value;
        row = row.nextSiblingElement(row.tagName());
    }

    delete serverCommand;
    return parameters;
}


ServerCommand *ServerAdapter::experimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType, int count)
{
    QString command = QString("expView -x %1").arg(experimentId);

    if(!modifiers.isEmpty()) {
        command.append(" -v " + modifiers.join(","));
    }

    if(!metrics.isEmpty()) {
        command.append(" -m " + metrics.join(","));
    }

    if(!experimentType.isEmpty()) {
        command.append(QString(" %1%2").arg(experimentType).arg(count));
    }

    return rawOpenSpeedShopCommand(command);
}
DataModel *ServerAdapter::waitExperimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType, int count)
{
    ServerCommand *serverCommand = experimentView(experimentId, modifiers, metrics, experimentType, count);
    waitCommand(serverCommand);

    DataModel *dataModel = NULL;
    try { dataModel = new DataModel(serverCommand->response(), this); }
    catch(QString err) { throw tr("Failed to create data model. Error from model: '%1'").arg(err); }

    delete serverCommand;
    return dataModel;
}

ServerCommand *ServerAdapter::experimentView(qint64 experimentId, QString experimentType, int count)
{
    return experimentView(experimentId, QStringList(), QStringList(), experimentType, count);
}
DataModel *ServerAdapter::waitExperimentView(qint64 experimentId, QString experimentType, int count)
{
    return waitExperimentView(experimentId, QStringList(), QStringList(), experimentType, count);
}


QList<ServerAdapter::Process> ServerAdapter::waitExperimentProcesses(qint64 experimentId)
{
    QString command = QString("expStatus -x %1").arg(experimentId);
    ServerCommand *serverCommand = rawOpenSpeedShopCommand(command);
    QDomElement responseElement = waitCommand(serverCommand);
    QStringList stringList = getStringList(responseElement);

    QList<Process> processes;
    //FIXME: I don't know why I can't use character sets.  Bug in Qt4.7.1?
    QRegExp regex("^\\s*-h (.+) -p (.+) -t (.+) -r (.+) \\((.+)\\)\\s*$");
    foreach(QString string, stringList) {
        if(regex.exactMatch(string)) {
            Process process;
            process.host = regex.cap(1);
            process.processId = regex.cap(2).toLongLong();
            process.threadId = regex.cap(3).toLongLong();
            process.rank = regex.cap(4).toLongLong();
            process.executable = regex.cap(5);
            processes.append(process);
        }
    }

    return processes;
}


/*! \fn ServerAdapter::loadMetrics()
    \brief This doesn't belong here, it should be retreived from the server.
    \todo This doesn't belong here, it should be retreived from the server.
 */
void ServerAdapter::loadMetrics()
{
    m_MetricsList = QDomDocument("Modifiers");
    QFile file("metrics.xml");
    if (!file.open(QIODevice::ReadOnly)) {
        throw tr("Could not open metrics description file");
    }
    if (!m_MetricsList.setContent(&file)) {
        file.close();
        throw tr("Could not use metrics description file after opening, possibly invalid text");
    }
    file.close();
}
QMap<QString,QString> ServerAdapter::waitExperimentTypeModifiers(QString experimentType)
{
    QMap<QString,QString> modifiers;

    if(m_MetricsList.isNull()) {
        loadMetrics();
    }

    //NOTE: This seems fast enough on its own, but we may have to do some caching later
    QDomElement element = m_MetricsList.firstChildElement("ExperimentTypes");
    element = element.firstChildElement("ExperimentType");
    while(!element.isNull()) {
        if(!element.attribute("value").compare(experimentType, Qt::CaseInsensitive)) {
            element = element.firstChildElement("Modifier");
            while(!element.isNull()) {
                modifiers[element.attribute("value")] = element.attribute("summary");
                element = element.nextSiblingElement(element.tagName());
            }
            return modifiers;
        }
        element = element.nextSiblingElement(element.tagName());
    }

    qDebug() << "Could not find experiment type";
    return modifiers;
}
QMap<QString,QString> ServerAdapter::waitExperimentTypeMetrics(QString experimentType)
{
    QMap<QString,QString> metrics;

    if(m_MetricsList.isNull()) {
        loadMetrics();
    }

    //NOTE: This seems fast enough on its own, but we may have to do some caching later
    QDomElement element = m_MetricsList.firstChildElement("ExperimentTypes");
    element = element.firstChildElement("ExperimentType");
    while(!element.isNull()) {
        if(!element.attribute("value").compare(experimentType, Qt::CaseInsensitive)) {
            element = element.firstChildElement("Metric");
            while(!element.isNull()) {
                metrics[element.attribute("value")] = element.attribute("summary");
                element = element.nextSiblingElement(element.tagName());
            }
            return metrics;
        }
        element = element.nextSiblingElement(element.tagName());
    }
    return metrics;
}

/*** END OpenSpeedShopCLI commands *******************************************/


} // namespace OpenSpeedShop
} // namespace Plugins
