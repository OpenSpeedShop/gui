#include "DirectAdapter.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QDebug>

#include <OpenSpeedShop/ConnectionManager/ConnectionManager.h>
#include <OpenSpeedShop/ConnectionManager/ServerCommand.h>
#include <OpenSpeedShop/ModelManager/DataModel.h>

using namespace Plugins::OpenSpeedShop;

namespace Plugins {
namespace DirectAdapter {

/*! \fn DirectAdapter::DirectAdapter()
    \brief
 */
DirectAdapter::DirectAdapter(QObject *parent) :
    IAdapter(parent)
{
    setObjectName("DirectAdapter");
}

bool DirectAdapter::isCompatible(QString directVersion)
{
    static const QRegExp regex = QRegExp(QString("^DirectConnection_.*$"), Qt::CaseInsensitive);
    return regex.exactMatch(directVersion);
}

/*! \fn DirectAdapter::rawCommand()
    \brief
 */
ServerCommand *DirectAdapter::rawCommand(QString command, QString type)
{
    ConnectionManager &connectionManager = ConnectionManager::instance();
    ServerCommand *serverCommand = new ServerCommand(command, type, this);
    if(!connectionManager.sendCommand(serverCommand)) {
        //! \todo Deal with a returned false value
    }
    return serverCommand;
}

/*! \fn DirectAdapter::rawServerCommand()
    \brief
 */
ServerCommand *DirectAdapter::rawServerCommand(QString command)
{
    return rawCommand(command, "Direct");
}

/*! \fn DirectAdapter::rawOpenSpeedShopCommand()
    \brief
 */
ServerCommand *DirectAdapter::rawOpenSpeedShopCommand(QString command)
{
    return rawCommand(command, "OpenSpeedShopCLI");
}

/*! \fn DirectAdapter::waitCommand()
    \brief Simply waits for a command to get a response, and returns the response document.

    According to the Qt4 docs, the QDomElement is stored in memory "efficiently," meaning that we can copy very
    quickly. This is why we're not messing with pointers here.
 */
QDomElement DirectAdapter::waitCommand(ServerCommand *serverCommand)
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

/*! \fn DirectAdapter::getString()
    \brief
 */
QString DirectAdapter::getString(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw tr("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw tr("'CommandObject' doesn't exist, as expected.");

    QDomElement child = commandObjectElement.firstChildElement("String");

    return child.attribute("value");
}

/*! \fn DirectAdapter::getStringList()
    \brief
 */
QStringList DirectAdapter::getStringList(QDomElement responseElement)
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

/*! \fn DirectAdapter::getInt()
    \brief
 */
qint64 DirectAdapter::getInt(QDomElement responseElement)
{
    QDomElement openSpeedShopCliElement = responseElement.firstChildElement("OpenSpeedShopCLI");
    if(openSpeedShopCliElement.isNull()) throw tr("'OpenSpeedShopCLI' doesn't exist, as expected.");

    QDomElement commandObjectElement = openSpeedShopCliElement.firstChildElement("CommandObject");
    if(commandObjectElement.isNull()) throw tr("'CommandObject' doesn't exist, as expected.");

    QDomElement child = commandObjectElement.firstChildElement("Int");

    return child.attribute("value").toLongLong();
}

/*! \fn DirectAdapter::getIntList()
    \brief
 */
QList<qint64> DirectAdapter::getIntList(QDomElement responseElement)
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


/*** BEGIN SocketDirect commands *********************************************/

/*! \fn DirectAdapter::version()
    \brief
 */
ServerCommand *DirectAdapter::version()
{
    return rawServerCommand("version");
}

/*! \fn DirectAdapter::waitVersion()
    \brief
 */
QString DirectAdapter::waitVersion()
{
    ServerCommand *serverCommand = version();
    while(serverCommand->state() != ServerCommand::State_Response)
        QApplication::processEvents();

    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement directResponseElement = responseElement.firstChildElement("DirectResponse");
    if(directResponseElement.isNull()) throw tr("'DirectResponse' doesn't exist, as expected.");

    if(!directResponseElement.hasAttribute("version"))
        throw tr("'DirectResponse' doesn't have 'version' attribute, as expected.");

    QString version = directResponseElement.attribute("version");
    serverCommand->deleteLater();
    return version;
}

ServerCommand *DirectAdapter::keepAlive()
{
    return rawServerCommand("keepalive");
}

int DirectAdapter::waitKeepAlive()
{
    ServerCommand *serverCommand = version();
    while(serverCommand->state() != ServerCommand::State_Response)
        QApplication::processEvents();

    QDomElement responseElement = waitCommand(serverCommand);

    QDomElement directResponseElement = responseElement.firstChildElement("DirectResponse");
    if(directResponseElement.isNull()) throw tr("'DirectResponse' doesn't exist, as expected.");

    if(!directResponseElement.hasAttribute("milliseconds"))
        throw tr("'DirectResponse' doesn't have 'version' attribute, as expected.");

    bool okay;
    int keepAliveMS = directResponseElement.attribute("milliseconds").toInt(&okay);
    serverCommand->deleteLater();

    if(!okay) throw tr("Could not convert 'milliseconds' attribute to int.");

    return keepAliveMS;
}

void DirectAdapter::doKeepAlive()
{
    QTimer::singleShot(waitKeepAlive(), this, SLOT(doKeepAlive()));
}


/*! \fn DirectAdapter::exit()
    \brief
 */
ServerCommand *DirectAdapter::exit()
{
    return rawServerCommand("exit");
}

/*! \fn DirectAdapter::waitExit()
    \brief
 */
void DirectAdapter::waitExit()
{
    ServerCommand *serverCommand = exit();
    waitCommand(serverCommand);
    serverCommand->deleteLater();
}

/*** END SocketDirect commands ***********************************************/

QStringList DirectAdapter::waitDirStat(QString path)
{
    QStringList retval;
    foreach(QFileInfo entry, QDir(path).entryInfoList()) {
        if(entry.fileName().startsWith(QLatin1Char('.'))) {
            continue;
        }

        if(entry.isDir()) {
            retval.append(entry.fileName() + "/");
        } else if(entry.isFile()) {
            retval.append(entry.fileName());
        } else {
            throw QString("Unknown filetype");
        }
    }

    return retval;
}

QString DirectAdapter::waitCatFile(QString filePath)
{
    QFile file(filePath);
    if(!file.exists()) {
        throw QString("File \"%1\" doens't exist").arg(filePath);
    }

    if(!file.open(QFile::ReadOnly)) {
        throw QString("File \"%1\" unable to open").arg(filePath);
    }

    QByteArray text = file.readAll();
    file.close();

    return QString(text);
}

bool DirectAdapter::waitFileExists(QString filePath)
{
    QFile file(filePath);
    return file.exists();
}


/*** BEGIN OpenSpeedShopCLI commands *****************************************/

/*! \fn DirectAdapter::ossHelp()
    \brief Good for testing purposes
 */
ServerCommand *DirectAdapter::ossHelp()
{
    return rawOpenSpeedShopCommand("help");
}

/*! \fn DirectAdapter::waitOssHelp()
    \brief Sychronous call to ossHelp(); good for testing purposes.
 */
QStringList DirectAdapter::waitOssHelp()
{
    ServerCommand *serverCommand = ossHelp();
    QDomElement responseElement = waitCommand(serverCommand);
    QStringList stringList = getStringList(responseElement);
    serverCommand->deleteLater();
    return stringList;
}

/*! \fn DirectAdapter::experimentTypes()
    \brief Lists experiment types available.
    \returns the asynchronous command object.
 */
ServerCommand *DirectAdapter::experimentTypes()
{
    QString command = QString("list -v expTypes -v all");
    return rawOpenSpeedShopCommand(command);
}

/*! \fn DirectAdapter::waitExperimentTypes()
    \brief Synchronously lists experiment types available.
    \returns A list of experiment types available.
 */
QStringList DirectAdapter::waitExperimentTypes()
{
    ServerCommand *serverCommand = experimentTypes();
    QDomElement responseElement = waitCommand(serverCommand);
    QStringList stringList = getStringList(responseElement);
    serverCommand->deleteLater();
    return stringList;
}

/*! \fn DirectAdapter::restore()
    \brief Opens an experiment from it's database file.
    \param filepath is the complete path to the database file.
    \returns the asynchronous command object.
 */
ServerCommand *DirectAdapter::restore(QString filepath)
{
    QString command = QString("expRestore -f %1").arg(filepath);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn DirectAdapter::waitRestore()
    \brief Synchronously opens an experiment from it's database file.
    \param filepath is the complete path to the database file.
    \returns The experiment identifier associated with the now open database file.
 */
QUuid DirectAdapter::waitRestore(QString filepath)
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

/*! \fn DirectAdapter::listOpenExperiments()
    \brief Lists the eperiment identifiers of currently open experiments.
    \returns the asynchronous command object.
 */
ServerCommand *DirectAdapter::listOpenExperiments()
{
    QString command = QString("list -v exp");
    return rawOpenSpeedShopCommand(command);
}

/*! \fn DirectAdapter::waitListOpenExperiments()
    \brief Synchronously lists the eperiment identifiers of currently open experiments.
    \returns The list of eperiment identifiers.
 */
QList<qint64> DirectAdapter::waitListOpenExperiments()
{
    ServerCommand *serverCommand = listOpenExperiments();
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getIntList(responseElement);
}

/*! \fn DirectAdapter::experimentTypes()
    \brief Lists the experiment types associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *DirectAdapter::experimentType(qint64 experimentId)
{
    QString command = QString("list -v expTypes -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn DirectAdapter::waitExperimentType()
    \brief Sycnronously lists the experiment types associated with an experiment.
    \param experimentId the experiment identifier.
    \returns a list of experiment type names associated with the experiment
 */
QString DirectAdapter::waitExperimentType(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentType(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString DirectAdapter::waitExperimentType(QUuid experimentUid)
{
    return waitExperimentType(experimentId(experimentUid));
}

/*! \fn DirectAdapter::experimentDatabase()
    \brief Lists the database filepath associated with an experiment
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *DirectAdapter::experimentDatabase(qint64 experimentId)
{
    QString command = QString("list -v database -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn DirectAdapter::waitExperimentDatabase()
    \brief Synchronously lists the database filepath associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the database filepath associated with the experiment.
 */
QString DirectAdapter::waitExperimentDatabase(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentDatabase(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString DirectAdapter::waitExperimentDatabase(QUuid experimentUid)
{
    return waitExperimentDatabase(experimentId(experimentUid));
}

/*! \fn DirectAdapter::experimentMetrics()
    \brief Lists the metrics associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *DirectAdapter::experimentMetrics(qint64 experimentId)
{
    QString command = QString("list -v metrics -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn DirectAdapter::waitExperimentMetrics()
    \brief Synchronously lists the metrics associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the list of metrics associated with the experiment.
 */
QStringList DirectAdapter::waitExperimentMetrics(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentMetrics(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList DirectAdapter::waitExperimentMetrics(QUuid experimentUid)
{
    return waitExperimentMetrics(experimentId(experimentUid));

}

/*! \fn DirectAdapter::experimentRanks()
    \brief Lists the ranks associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the asynchronous command object
 */
ServerCommand *DirectAdapter::experimentRanks(qint64 experimentId)
{
    QString command = QString("list -v ranks -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}

/*! \fn DirectAdapter::waitExperimentRanks()
    \brief Synchronously lists the ranks associated with an experiment.
    \param experimentId the experiment identifier.
    \returns the list of ranks associated with the experiment.
 */
QList<qint64> DirectAdapter::waitExperimentRanks(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentRanks(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getIntList(responseElement);
}
QList<qint64> DirectAdapter::waitExperimentRanks(QUuid experimentUid)
{
    return waitExperimentRanks(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentAppCommand(qint64 experimentId)
{
    QString command = QString("list -v appcommand -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QString DirectAdapter::waitExperimentAppCommand(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentAppCommand(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString DirectAdapter::waitExperimentAppCommand(QUuid experimentUid)
{
    return waitExperimentAppCommand(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentExecutable(qint64 experimentId)
{
    QString command = QString("list -v executable -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QString DirectAdapter::waitExperimentExecutable(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentExecutable(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getString(responseElement);
}
QString DirectAdapter::waitExperimentExecutable(QUuid experimentUid)
{
    return waitExperimentExecutable(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentHosts(qint64 experimentId)
{
    QString command = QString("list -v hosts -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QStringList DirectAdapter::waitExperimentHosts(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentHosts(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList DirectAdapter::waitExperimentHosts(QUuid experimentUid)
{
    return waitExperimentHosts(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentPids(qint64 experimentId)
{
    QString command = QString("list -v pids -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QList<qint64> DirectAdapter::waitExperimentPids(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentPids(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getIntList(responseElement);
}
QList<qint64> DirectAdapter::waitExperimentPids(QUuid experimentUid)
{
    return waitExperimentPids(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentSourceFiles(qint64 experimentId)
{
    QString command = QString("list -v srcfullpath -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QStringList DirectAdapter::waitExperimentSourceFiles(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentSourceFiles(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList DirectAdapter::waitExperimentSourceFiles(QUuid experimentUid)
{
    return waitExperimentSourceFiles(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentObjectFiles(qint64 experimentId)
{
    QString command = QString("list -v src -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QStringList DirectAdapter::waitExperimentObjectFiles(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentObjectFiles(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getStringList(responseElement);
}
QStringList DirectAdapter::waitExperimentObjectFiles(QUuid experimentUid)
{
    return waitExperimentObjectFiles(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentThreads(qint64 experimentId)
{
    QString command = QString("list -v threads -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QList<qint64> DirectAdapter::waitExperimentThreads(qint64 experimentId)
{
    ServerCommand *serverCommand = experimentThreads(experimentId);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getIntList(responseElement);
}
QList<qint64> DirectAdapter::waitExperimentThreads(QUuid experimentUid)
{
    return waitExperimentThreads(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentViews(QString experimentType)
{
    QString command = QString("list -v views %1").arg(experimentType);
    return rawOpenSpeedShopCommand(command);
}
QStringList DirectAdapter::waitExperimentViews(QString experimentType)
{
    ServerCommand *serverCommand = experimentViews(experimentType);
    QDomElement responseElement = waitCommand(serverCommand);
    serverCommand->deleteLater();
    return getStringList(responseElement);
}

ServerCommand *DirectAdapter::experimentParameterValues(qint64 experimentId)
{
    QString command = QString("list -v paramvalues -x %1").arg(experimentId);
    return rawOpenSpeedShopCommand(command);
}
QMap<QString, QVariant> DirectAdapter::waitExperimentParameterValues(qint64 experimentId)
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
QMap<QString, QVariant> DirectAdapter::waitExperimentParameterValues(QUuid experimentUid)
{
    return waitExperimentParameterValues(experimentId(experimentUid));
}

ServerCommand *DirectAdapter::experimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType, int count)
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
QAbstractItemModel *DirectAdapter::waitExperimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType, int count)
{
    ServerCommand *serverCommand = experimentView(experimentId, modifiers, metrics, experimentType, count);
    waitCommand(serverCommand);

    DataModel *dataModel = NULL;
    try { dataModel = new DataModel(serverCommand->response(), this); }
    catch(QString err) { throw tr("Failed to create data model. Error from model: '%1'").arg(err); }

    serverCommand->deleteLater();
    return dataModel;
}
QAbstractItemModel *DirectAdapter::waitExperimentView(QUuid experimentUid, QStringList modifiers, QStringList metrics, int count)
{
    QString experimentType = waitExperimentType(experimentUid);
    return waitExperimentView(experimentId(experimentUid), modifiers, metrics, experimentType, count);
}

ServerCommand *DirectAdapter::experimentView(qint64 experimentId, QString experimentType, int count)
{
    return experimentView(experimentId, QStringList(), QStringList(), experimentType, count);
}
QAbstractItemModel *DirectAdapter::waitExperimentView(qint64 experimentId, QString experimentType, int count)
{
    return waitExperimentView(experimentId, QStringList(), QStringList(), experimentType, count);
}
QAbstractItemModel *DirectAdapter::waitExperimentView(QUuid experimentUid, int count)
{
    QString experimentType = waitExperimentType(experimentUid);
    return waitExperimentView(experimentId(experimentUid), experimentType, count);
}

QList<DirectAdapter::Process> DirectAdapter::waitExperimentProcesses(qint64 experimentId)
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
QList<DirectAdapter::Process> DirectAdapter::waitExperimentProcesses(QUuid experimentUid)
{
    return waitExperimentProcesses(experimentId(experimentUid));
}

/*! \fn DirectAdapter::loadMetrics()
    \brief This doesn't belong here, it should be retreived from the direct.
    \todo This doesn't belong here, it should be retreived from the direct.
 */
void DirectAdapter::loadMetrics()
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
QMap<QString,QString> DirectAdapter::waitExperimentTypeModifiers(QString experimentType)
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

#ifdef DIRECTADAPTER_DEBUG
    qDebug() << __FILE__ << ":" << __LINE__ << "Could not find experiment type in modifiers file" << experimentType;
#endif

    return modifiers;
}

QMap<QString,QString> DirectAdapter::waitExperimentTypeMetrics(QString experimentType)
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

#ifdef DIRECTADAPTER_DEBUG
    qDebug() << __FILE__ << ":" << __LINE__ << "Could not find experiment type in metrics file" << experimentType;
#endif

    return metrics;
}

/*** END OpenSpeedShopCLI commands *******************************************/

int DirectAdapter::experimentId(QUuid experimentUid)
{
    return m_ExperimentIds.value(experimentUid, -1);
}
QUuid DirectAdapter::experimentUid(qint64 experimentId)
{
    return m_ExperimentIds.key(experimentId, QUuid());
}


} // namespace DirectAdapter
} // namespace Plugins
