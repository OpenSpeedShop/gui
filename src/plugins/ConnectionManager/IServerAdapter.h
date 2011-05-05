#ifndef ISERVERADAPTER_H
#define ISERVERADAPTER_H

#include <QObject>

namespace Plugins {
namespace ConnectionManager {

class ConnectionManager;
class ServerCommand;

enum ExperimentModes {
    ExperimentMode_Offline,
    ExperimentMode_Online
};

class IServerAdapter : public QObject
{
    Q_OBJECT
public:
    explicit IServerAdapter(QObject *parent = 0);

    void rawCommand(QString command);


//    void attach();
//    void close();
//    virtual qint64 create(ExperimentModes experimentMode = ExperimentMode_Offline, bool mpi = false, QStringList targets, QString ExperimentType) = 0;
//    void detach();
//    void disable();
//    void enable();
//    void focus();
//    void go();
//    void pause();
//    void restore();
//    void save();
//    void setParameter();

//    void status();
//    void view();

//    void listBreaks();
//    void listExperiments();
//    void listHosts();
//    void listObjects();
//    void listProcessIDs();
//    void listMetrics();
//    void listParams();
//    void listRanks();
//    void listSource();
//    void listStatus();
//    void listThreads();
//    void listTypes();
//    void listViews();

//    void listAllMetrics();
//    void listAllParams();
//    void listAllTypes();
//    void listAllViews();

//    void setBreak();
//    void clearBreak();
//    void exit();
//    void help();
//    void history();
//    void log();
//    void openGUI();
//    void playBack();
//    void record();

signals:

public slots:

protected:


};

} // namespace ConnectionManager
} // namespace Plugins

#endif // ISERVERADAPTER_H
