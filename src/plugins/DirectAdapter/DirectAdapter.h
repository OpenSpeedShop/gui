#ifndef DIRECTADAPTER_H
#define DIRECTADAPTER_H

#include <QtCore>
#include <QtXml>

#include <OpenSpeedShop/ConnectionManager/IAdapter.h>

namespace Plugins {

namespace OpenSpeedShop {
    class ServerCommand;
}

namespace DirectAdapter {

// Only way to get a calling thread to sleep using Qt4.
class Thread : public QThread
{
public:
    static void sleep(unsigned long msecs) { QThread::msleep(msecs); }
};

class DirectAdapter : public Plugins::OpenSpeedShop::IAdapter
{
Q_OBJECT
Q_INTERFACES(Plugins::OpenSpeedShop::IAdapter)

public:
    explicit DirectAdapter(QObject *parent = 0);

    bool isCompatible(QString directVersion);

    QString waitVersion();
    int waitKeepAlive();
    void waitExit();

    QStringList waitDirStat(QString path);
    QString waitCatFile(QString filePath);
    bool waitFileExists(QString filePath);

    QStringList waitOssHelp();
    QStringList waitExperimentTypes();
    QUuid waitRestore(QString filepath);
    QList<qint64> waitListOpenExperiments();
    QString waitExperimentType(QUuid experimentUid);
    QString waitExperimentDatabase(QUuid experimentUid);
    QStringList waitExperimentMetrics(QUuid experimentUid);
    QList<qint64> waitExperimentRanks(QUuid experimentUid);
    QString waitExperimentAppCommand(QUuid experimentUid);
    QString waitExperimentExecutable(QUuid experimentUid);
    QStringList waitExperimentHosts(QUuid experimentUid);
    QList<qint64> waitExperimentPids(QUuid experimentUid);
    QStringList waitExperimentSourceFiles(QUuid experimentUid);
    QStringList waitExperimentObjectFiles(QUuid experimentUid);
    QList<qint64> waitExperimentThreads(QUuid experimentUid);
    QStringList waitExperimentViews(QString experimentType);
    QMap<QString, QVariant> waitExperimentParameterValues(QUuid experimentUid);
    QAbstractItemModel *waitExperimentView(QUuid experimentUid, int count = 1000);
    QAbstractItemModel *waitExperimentView(QUuid experimentUid, QStringList modifiers, QStringList metrics, int count = 1000);
    QList<Plugins::OpenSpeedShop::IAdapter::Process> waitExperimentProcesses(QUuid experimentUid);
    QMap<QString,QString> waitExperimentTypeModifiers(QString experimentType);
    QMap<QString,QString> waitExperimentTypeMetrics(QString experimentType);

protected:
    Plugins::OpenSpeedShop::ServerCommand *version();
    Plugins::OpenSpeedShop::ServerCommand *keepAlive();
    Plugins::OpenSpeedShop::ServerCommand *exit();

    Plugins::OpenSpeedShop::ServerCommand *ossHelp();
    Plugins::OpenSpeedShop::ServerCommand *experimentTypes();
    Plugins::OpenSpeedShop::ServerCommand *restore(QString filepath);
    Plugins::OpenSpeedShop::ServerCommand *listOpenExperiments();
    Plugins::OpenSpeedShop::ServerCommand *experimentType(qint64 experimentId);
    QString waitExperimentType(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentDatabase(qint64 experimentId);
    QString waitExperimentDatabase(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentMetrics(qint64 experimentId);
    QStringList waitExperimentMetrics(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentRanks(qint64 experimentId);
    QList<qint64> waitExperimentRanks(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentAppCommand(qint64 experimentId);
    QString waitExperimentAppCommand(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentExecutable(qint64 experimentId);
    QString waitExperimentExecutable(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentHosts(qint64 experimentId);
    QStringList waitExperimentHosts(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentPids(qint64 experimentId);
    QList<qint64> waitExperimentPids(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentSourceFiles(qint64 experimentId);
    QStringList waitExperimentSourceFiles(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentObjectFiles(qint64 experimentId);
    QStringList waitExperimentObjectFiles(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentThreads(qint64 experimentId);
    QList<qint64> waitExperimentThreads(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentViews(QString experimentType);
    Plugins::OpenSpeedShop::ServerCommand *experimentParameterValues(qint64 experimentId);
    QMap<QString, QVariant> waitExperimentParameterValues(qint64 experimentId);
    Plugins::OpenSpeedShop::ServerCommand *experimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    QAbstractItemModel *waitExperimentView(qint64 experimentId, QString experimentType = QString(), int count = 1000);
    Plugins::OpenSpeedShop::ServerCommand *experimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);
    QAbstractItemModel *waitExperimentView(qint64 experimentId, QStringList modifiers, QStringList metrics, QString experimentType = QString(), int count = 1000);
    QList<Plugins::OpenSpeedShop::IAdapter::Process> waitExperimentProcesses(qint64 experimentId);

    Plugins::OpenSpeedShop::ServerCommand *rawCommand(QString command, QString type);
    Plugins::OpenSpeedShop::ServerCommand *rawServerCommand(QString command);
    Plugins::OpenSpeedShop::ServerCommand *rawOpenSpeedShopCommand(QString command);
    QDomElement waitCommand(Plugins::OpenSpeedShop::ServerCommand *ServerCommand);
    QString getString(QDomElement responseElement);
    QStringList getStringList(QDomElement responseElement);
    qint64 getInt(QDomElement responseElement);
    QList<qint64> getIntList(QDomElement responseElement);

    int experimentId(QUuid experimentUid);
    QUuid experimentUid(qint64 experimentId);

    void loadMetrics();

protected slots:
    void doKeepAlive();

private:
    QDomDocument m_MetricsList;
    QHash<QUuid, int> m_ExperimentIds;

};

} // namespace DirectAdapter
} // namespace Plugins

#endif // DIRECTADAPTER_H
