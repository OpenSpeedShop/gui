#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <MainWindow/MainWindow.h>
#include "ConnectionManager/ConnectionManager.h"
#include "ConnectionManager/ServerAdapter.h"
#include "ConnectionManager/ServerCommand.h"
#include <QDomDocument>

#include "ModelManager/ModelDescriptorWidget.h"

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace OpenSpeedShop {

ExperimentWidget::ExperimentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentWidget)
{
    ui->setupUi(this);

    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();

    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify("Server not connected");
        return;
    }

    QStringList experimentTypes = serverAdapter->waitExperimentTypes();
    if(!experimentTypes.isEmpty()) {
        ui->cmbExperimentTypes->addItems(experimentTypes);
    }
}

ExperimentWidget::~ExperimentWidget()
{
    delete ui;
}

void ExperimentWidget::create()
{
}

void ExperimentWidget::load()
{
    // Grab the current server adapter
    // Ensure that we're connected to a server
    // Start sending commands

    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();

    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify("Server not connected");
        return;
    }

//    qDebug() << serverAdapter->waitVersion();

    QUuid expId = serverAdapter->waitRestore("/home/dane/smg2000-io.openss");
    ui->txtDatabasePath->setText(serverAdapter->waitExperimentDatabase(expId));
    ui->txtExecutablePath->setText(serverAdapter->waitExperimentExecutable(expId));
    ui->txtCommand->setText(serverAdapter->waitExperimentAppCommand(expId));

    ui->cmbExperimentTypes->setCurrentIndex(ui->cmbExperimentTypes->findText(serverAdapter->waitExperimentTypes(expId)));

//    qDebug() << "parameters" << serverAdapter->waitExperimentParameterValues(expId);

//    qDebug() << "hosts" << serverAdapter->waitExperimentHosts(expId);
//    qDebug() << "ranks" << serverAdapter->waitExperimentRanks(expId);
//    qDebug() << "pids" << serverAdapter->waitExperimentPids(expId);

//    qDebug() << "sources" << serverAdapter->waitExperimentSourceFiles(expId);
//    qDebug() << "objects" << serverAdapter->waitExperimentObjectFiles(expId);
//    qDebug() << "threads" << serverAdapter->waitExperimentThreads(expId);

//    qDebug() << "Processes:";
//    QList<ServerAdapter::Process> processes = serverAdapter->experimentProcesses(expId);
//    foreach(ServerAdapter::Process process, processes) {
//        qDebug() << "\t" << process.host << process.processId << process.threadId << process.rank << process.executable;
//    }


//    QString experimentType = serverAdapter->waitExperimentTypes(expId);

//    qDebug() << "Modifiers" << serverAdapter->waitExperimentTypeModifiers(experimentType);
//    qDebug() << "Metrics" << serverAdapter->waitExperimentTypeMetrics(experimentType);

//    QStringList modifiers;
//    modifiers << "CallTrees" << "FullStack";
//    QStringList metrics;
//    metrics << "counts" << "io::exclusive_times"; // << "io::inclusive_times";

//    try {
//    DataModel *dataModel = serverAdapter->waitExperimentView(expId, modifiers, metrics, experimentType, 10);
//    ui->treeView->setModel(dataModel);
//    } catch(QString err) {
//        using namespace Core::MainWindow;
//        QString errorMessage = tr("An error occured while loading the experiment view: '%1'").arg(err);
//        MainWindow::instance()->notify(errorMessage, NotificationWidget::Critical);
//    }

//    serverAdapter->waitExit();

}

void ExperimentWidget::on_btnAddModel_clicked()
{
}


} // namespace OpenSpeedShop
} // namespace Plugins
