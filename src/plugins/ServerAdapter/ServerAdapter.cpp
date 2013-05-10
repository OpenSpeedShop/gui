#include "ServerAdapter.h"

#include <OpenSpeedShop/ConnectionManager/ConnectionManager.h>
#include <OpenSpeedShop/ConnectionManager/ServerCommand.h>
#include <OpenSpeedShop/ModelManager/DataModel.h>

#ifdef SERVERADAPTER_DEBUG
#  include <QDebug>
#endif

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace ServerAdapter {

/*! \fn ServerAdapter::ServerAdapter()
    \brief
 */
ServerAdapter::ServerAdapter(QObject *parent) :
    IAdapter(parent)
{
    setObjectName("ServerAdapter");
}

bool ServerAdapter::isCompatible(QString serverVersion)
{
    static const QRegExp regex = QRegExp(QString("^ServerConnection_.*$"), Qt::CaseInsensitive);
    return regex.exactMatch(serverVersion);
}

/*! \fn ServerAdapter::rawCommand()
    \brief
 */
ServerCommand *ServerAdapter::rawCommand(QString command, QString type)
{
    ConnectionManager &connectionManager = ConnectionManager::instance();
    ServerCommand *serverCommand = new ServerCommand(command, type, this);
    if(!connectionManager.sendCommand(serverCommand)) {
        //! \todo Deal with a returned false value
    }
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
    /*! \todo We need to notify the user of the wait state after the first second or so (spinning visual display).
        \todo We need to ask the user if they'd like to continue waiting after 30 seconds or so. */

    while(serverCommand->state() != ServerCommand::State_Response) {
        QApplication::processEvents();
        if(serverCommand->state() == ServerCommand::State_Invalid) {
            throw tr("Command has been invalidated");
        }
        Thread::sleep(10);
    }

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

    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement serverResponseElement = responseElement.firstChildElement("ServerResponse");
    if(serverResponseElement.isNull()) throw tr("'ServerResponse' doesn't exist, as expected.");

    if(!serverResponseElement.hasAttribute("version"))
        throw tr("'ServerResponse' doesn't have 'version' attribute, as expected.");

    QString version = serverResponseElement.attribute("version");
    serverCommand->deleteLater();
    return version;
}

ServerCommand *ServerAdapter::keepAlive()
{
    return rawServerCommand("keepalive");
}

int ServerAdapter::waitKeepAlive()
{
    ServerCommand *serverCommand = version();
    while(serverCommand->state() != ServerCommand::State_Response)
        QApplication::processEvents();

    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement serverResponseElement = responseElement.firstChildElement("ServerResponse");
    if(serverResponseElement.isNull()) throw tr("'ServerResponse' doesn't exist, as expected.");

    if(!serverResponseElement.hasAttribute("milliseconds"))
        throw tr("'ServerResponse' doesn't have 'version' attribute, as expected.");

    bool okay;
    int keepAliveMS = serverResponseElement.attribute("milliseconds").toInt(&okay);
    serverCommand->deleteLater();

    if(!okay) throw tr("Could not convert 'milliseconds' attribute to int.");

    return keepAliveMS;
}

void ServerAdapter::doKeepAlive()
{
    QTimer::singleShot(waitKeepAlive(), this, SLOT(doKeepAlive()));
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
    waitCommand(serverCommand);
    serverCommand->deleteLater();
}

/*** END SocketServer commands ***********************************************/

QStringList ServerAdapter::waitDirStat(QString path)
{
    QStringList listing;

    //DEBUG:  This is for debugging only
    ServerCommand *serverCommand = rawCommand(QString("dirStat %1").arg(path), "FileSystem");
    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement fileSystemElement = responseElement.firstChildElement("FileSystem");
    if(fileSystemElement.isNull()) throw tr("'FileSystem' element doesn't exist, as expected.");

    QDomElement directoryElement = fileSystemElement.firstChildElement("Dir");
    if(directoryElement.isNull()) {

        // Deal with other responses, like errors
        QDomElement exceptionElement = fileSystemElement.firstChildElement("Exception");
        if(!exceptionElement.isNull()) {
            throw tr("Exception returned from server: '%1'").arg(exceptionElement.text());
        }

        throw tr("'Dir' element doesn't exist, as expected. No error was given from the server.");
    }

    QDomElement fileElement = directoryElement.firstChildElement("File");
    while(!fileElement.isNull()) {
        QString fileName = fileElement.attribute("name");

        // Ignore hidden files
        if(fileName.startsWith(QLatin1Char('.'))) {
            fileElement = fileElement.nextSiblingElement(fileElement.tagName());
            continue;
        }

        listing.append(fileName);
        fileElement = fileElement.nextSiblingElement(fileElement.tagName());
    }

    serverCommand->deleteLater();
    return listing;
}

QString ServerAdapter::waitCatFile(QString filePath)
{
    QString fileContents;

    ServerCommand *serverCommand = rawCommand(QString("catFile %1").arg(filePath), "FileSystem");
    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement fileSystemElement = responseElement.firstChildElement("FileSystem");
    if(fileSystemElement.isNull()) throw tr("'FileSystem' element doesn't exist, as expected.");

    QDomElement fileContentsElement = fileSystemElement.firstChildElement("FileContent");
    if(fileContentsElement.isNull()) {

        // Deal with other responses, like errors
        QDomElement exceptionElement = fileSystemElement.firstChildElement("Exception");
        if(!exceptionElement.isNull()) {
            throw tr("Exception returned from server: '%1'").arg(exceptionElement.text());
        }

        throw tr("'FileContent' element doesn't exist, as expected. No error was given from the server.");
    }

    fileContents = fileContentsElement.firstChild().toText().nodeValue();

    serverCommand->deleteLater();
    return fileContents;
}

bool ServerAdapter::waitFileExists(QString filePath)
{
    ServerCommand *serverCommand = rawCommand(QString("fileExists %1").arg(filePath), "FileSystem");
    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement fileSystemElement = responseElement.firstChildElement("FileSystem");
    if(fileSystemElement.isNull()) throw tr("'FileSystem' element doesn't exist, as expected.");

    QDomElement fileExistsElement = fileSystemElement.firstChildElement("FileExists");
    if(fileExistsElement.isNull()) {

        // Deal with other responses, like errors
        QDomElement exceptionElement = fileSystemElement.firstChildElement("Exception");
        if(!exceptionElement.isNull()) {
            throw tr("Exception returned from server: '%1'").arg(exceptionElement.text());
        }

        throw tr("'FileExists' element doesn't exist, as expected. No error was given from the server.");
    }


    QString fileExists = fileExistsElement.attribute("exists", "false");
    serverCommand->deleteLater();
    return (fileExists == "true");
}


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
    serverCommand->deleteLater();
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
    serverCommand->deleteLater();
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
QUuid ServerAdapter::waitRestore(QString filepath)
{
    ServerCommand *serverCommand = restore(filepath);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();

    // Associate an experiment UID with the experiment ID, and return it
    qint64 experimentId = getInt(responseElement);
    QUuid experimentUid = QUuid::createUuid();
    m_ExperimentIds.insert(experimentUid, experimentId);
    return experimentUid;
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
    serverCommand->deleteLater();
    return getIntList(responseElement);
}

/*! \fn ServerAdapter::experimentTypes()
    \brief Lists the experiment types associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *ServerAdapter::experimentType(qint64 experimentId)
{
    QString command = QString("list -v expTypes -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn ServerAdapter::waitExperimentType()
    \brief Sycnronously lists the experiment types associated with an experiment.
    \param experimentId the experiment identifier.
    \returns a list of experiment type names associated with the experiment
 */
QString ServerAdapter::waitExperimentType(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentType(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString ServerAdapter::waitExperimentType(QUuid experimentUid)
{
    return waitExperimentType(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString ServerAdapter::waitExperimentDatabase(QUuid experimentUid)
{
    return waitExperimentDatabase(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList ServerAdapter::waitExperimentMetrics(QUuid experimentUid)
{
    return waitExperimentMetrics(experimentId(experimentUid));

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
    serverCommand->deleteLater();
    return getIntList(responseElement);
}
QList<qint64> ServerAdapter::waitExperimentRanks(QUuid experimentUid)
{
    return waitExperimentRanks(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString ServerAdapter::waitExperimentAppCommand(QUuid experimentUid)
{
    return waitExperimentAppCommand(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString ServerAdapter::waitExperimentExecutable(QUuid experimentUid)
{
    return waitExperimentExecutable(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList ServerAdapter::waitExperimentHosts(QUuid experimentUid)
{
    return waitExperimentHosts(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getIntList(responseElement);
}
QList<qint64> ServerAdapter::waitExperimentPids(QUuid experimentUid)
{
    return waitExperimentPids(experimentId(experimentUid));
}

ServerCommand *ServerAdapter::experimentSourceFiles(qint64 experimentId)
{
    QString command = QString("list -v srcfullpath -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QStringList ServerAdapter::waitExperimentSourceFiles(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentSourceFiles(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList ServerAdapter::waitExperimentSourceFiles(QUuid experimentUid)
{
    return waitExperimentSourceFiles(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList ServerAdapter::waitExperimentObjectFiles(QUuid experimentUid)
{
    return waitExperimentObjectFiles(experimentId(experimentUid));
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
    serverCommand->deleteLater();
    return getIntList(responseElement);
}
QList<qint64> ServerAdapter::waitExperimentThreads(QUuid experimentUid)
{
    return waitExperimentThreads(experimentId(experimentUid));
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
    serverCommand->deleteLater();
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

    serverCommand->deleteLater();
    return parameters;
}
QMap<QString, QVariant> ServerAdapter::waitExperimentParameterValues(QUuid experimentUid)
{
    return waitExperimentParameterValues(experimentId(experimentUid));
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
QAbstractItemModel *ServerAdapter::waitExperimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType, int count)
{
    ServerCommand *serverCommand = experimentView(experimentId, modifiers, metrics, experimentType, count);
    waitCommand(serverCommand);

    DataModel *dataModel = NULL;
    try { dataModel = new DataModel(serverCommand->response(), this); }
    catch(QString err) { throw tr("Failed to create data model. Error from model: '%1'").arg(err); }

    serverCommand->deleteLater();
    return dataModel;
}
QAbstractItemModel *ServerAdapter::waitExperimentView(QUuid experimentUid, QStringList modifiers, QStringList metrics, int count)
{
    QString experimentType = waitExperimentType(experimentUid);
    return waitExperimentView(experimentId(experimentUid), modifiers, metrics, experimentType, count);
}

ServerCommand *ServerAdapter::experimentView(qint64 experimentId, QString experimentType, int count)
{
    return experimentView(experimentId, QStringList(), QStringList(), experimentType, count);
}
QAbstractItemModel *ServerAdapter::waitExperimentView(qint64 experimentId, QString experimentType, int count)
{
    return waitExperimentView(experimentId, QStringList(), QStringList(), experimentType, count);
}
QAbstractItemModel *ServerAdapter::waitExperimentView(QUuid experimentUid, int count)
{
    QString experimentType = waitExperimentType(experimentUid);
    return waitExperimentView(experimentId(experimentUid), experimentType, count);
}

QList<ServerAdapter::Process> ServerAdapter::waitExperimentProcesses(qint64 experimentId)
{
    QString command = QString("expStatus -x %1").arg(experimentId);
//    QString command = QString("expStatus -v full -x %1").arg(experimentId);
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
QList<ServerAdapter::Process> ServerAdapter::waitExperimentProcesses(QUuid experimentUid)
{
    return waitExperimentProcesses(experimentId(experimentUid));
}

/*! \fn ServerAdapter::loadMetrics()
    \brief This doesn't belong here, it should be retreived from the server.
    \todo This doesn't belong here, it should be retreived from the server.
 */
void ServerAdapter::loadMetrics()
{
    m_MetricsList = QDomDocument("Modifiers");

    //FIXME: This should not be hard coded here
#ifdef Q_OS_WIN
    QFile file(QString("%1/oss/ExperimentTypes.xml").arg(QApplication::instance()->applicationDirPath()));
#else
    QFile file(QString("%1/../share/openspeedshop/gui/ExperimentTypes.xml").arg(QApplication::instance()->applicationDirPath()));
#endif

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
                modifiers.insert(element.attribute("value"), element.attribute("summary"));
                element = element.nextSiblingElement(element.tagName());
            }
            return modifiers;
        }
        element = element.nextSiblingElement(element.tagName());
    }

#ifdef SERVERADAPTER_DEBUG
    qDebug() << __FILE__ << ":" << __LINE__ << "Could not find experiment type in modifiers file" << experimentType;
#endif

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
                metrics.insert(element.attribute("value"), element.attribute("summary"));
                element = element.nextSiblingElement(element.tagName());
            }
            return metrics;
        }
        element = element.nextSiblingElement(element.tagName());
    }

#ifdef SERVERADAPTER_DEBUG
    qDebug() << __FILE__ << ":" << __LINE__ << "Could not find experiment type in metrics file" << experimentType;
#endif

    return metrics;
}

/*** END OpenSpeedShopCLI commands *******************************************/

int ServerAdapter::experimentId(QUuid experimentUid)
{
    return m_ExperimentIds.value(experimentUid, -1);
}
QUuid ServerAdapter::experimentUid(qint64 experimentId)
{
    return m_ExperimentIds.key(experimentId, QUuid());
}


} // namespace ServerAdapter
} // namespace Plugins
