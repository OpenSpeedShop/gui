#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <MainWindow/MainWindow.h>
#include "ConnectionManager/ConnectionManager.h"
#include "ConnectionManager/ServerAdapter.h"
#include "ConnectionManager/ServerCommand.h"
#include <QDomDocument>

#include "ViewManager/TreeView.h"

#include "ModelManager/ModelManager.h"
#include "ModelManager/ModelDescriptorListWidget.h"


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

    m_CurrentView = new TreeView(this);
    ui->grpView->layout()->addWidget(m_CurrentView);

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

//    m_ExperimentUid = serverAdapter->waitRestore("/home/dane/smg2000-pcsamp.openss");
    m_ExperimentUid = serverAdapter->waitRestore("/home/dane/smg2000-io.openss");

    ui->txtDatabasePath->setText(serverAdapter->waitExperimentDatabase(m_ExperimentUid));
    ui->txtExecutablePath->setText(serverAdapter->waitExperimentExecutable(m_ExperimentUid));
    ui->txtCommand->setText(serverAdapter->waitExperimentAppCommand(m_ExperimentUid));

    QString experimentType = serverAdapter->waitExperimentTypes(m_ExperimentUid);
    ui->cmbExperimentTypes->setCurrentIndex(ui->cmbExperimentTypes->findText(experimentType));

    ModelDescriptorListWidget *listWidget = ModelManager::instance()->createDescriptorListWidget(ui->modelListViewParent);
    listWidget->setExperimentType(experimentType);
    ui->modelListViewParent->layout()->addWidget(listWidget);
    connect(listWidget, SIGNAL(doubleClicked(QUuid)), this, SLOT(getModel(QUuid)));

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


//    serverAdapter->waitExit();

}

void ExperimentWidget::getModel(QUuid descriptorUid)
{
    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();

    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify("Server not connected");
        return;
    }

    try {
        QAbstractItemModel *dataModel = ModelManager::instance()->model(descriptorUid, m_ExperimentUid);
        m_CurrentView->setModel(dataModel);
    } catch(QString err) {
        using namespace Core::MainWindow;
        QString errorMessage = tr("An error occured while loading the experiment view: '%1'").arg(err);
        MainWindow::instance()->notify(errorMessage, NotificationWidget::Critical);
    }
}


void ExperimentWidget::on_btnAddModel_clicked()
{
}


} // namespace OpenSpeedShop
} // namespace Plugins
