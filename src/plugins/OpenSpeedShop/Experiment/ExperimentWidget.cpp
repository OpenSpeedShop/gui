#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <MainWindow/MainWindow.h>
#include "ConnectionManager/ConnectionManager.h"
#include "ConnectionManager/ServerAdapter.h"
#include "ConnectionManager/ServerCommand.h"
#include <QDomDocument>

#include "ViewManager/TreeView.h"

#include "ModelManager/ModelManager.h"
#include "ModelManager/ModelManagerDialog.h"
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
    ui->tabWidget->setCurrentIndex(0);

    m_CurrentView = new TreeView(this);
    ui->grpView->layout()->addWidget(m_CurrentView);

    IAdapter *serverAdapter = ConnectionManager::instance()->askAdapter();
    if(!serverAdapter) throw tr("Server not connected");

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
    ui->tabProperties->setEnabled(false);

    // Grab the current server adapter
    // Ensure that we're connected to a server
    // Start sending commands

    IAdapter *serverAdapter = ConnectionManager::instance()->askAdapter();
    if(!serverAdapter) throw tr("Server not connected");

//    qDebug() << serverAdapter->waitVersion();

//    m_ExperimentUid = serverAdapter->waitRestore("/home/dane/smg2000-pcsamp.openss");
    m_ExperimentUid = serverAdapter->waitRestore("/home/dane/smg2000-io.openss");

    ui->txtDatabasePath->setText(serverAdapter->waitExperimentDatabase(m_ExperimentUid));
    ui->txtExecutablePath->setText(serverAdapter->waitExperimentExecutable(m_ExperimentUid));
    ui->txtCommand->setText(serverAdapter->waitExperimentAppCommand(m_ExperimentUid));

    QString experimentType = serverAdapter->waitExperimentType(m_ExperimentUid);
    ui->cmbExperimentTypes->setCurrentIndex(ui->cmbExperimentTypes->findText(experimentType));

    loadModelDescriptors(experimentType);

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

void ExperimentWidget::loadModelDescriptors(QString experimentType)
{
    IAdapter *serverAdapter = ConnectionManager::instance()->askAdapter();
    if(!serverAdapter) throw tr("Server not connected");

    //TODO: Remove any previous model descriptor lists.

    /* Create and initialize the model descriptor list. */
    ModelDescriptorListWidget *listWidget = ModelManager::instance()->createDescriptorListWidget(ui->modelListViewParent);
    listWidget->setHeaderHidden(true);
    listWidget->setExperimentType(experimentType);
    ui->modelListViewParent->layout()->addWidget(listWidget);
    connect(listWidget, SIGNAL(currentDescriptorChanged(QUuid)), this, SLOT(getModel(QUuid)));
}

void ExperimentWidget::getModel(QUuid descriptorUid)
{
    try {

        IAdapter *serverAdapter = ConnectionManager::instance()->askAdapter();
        if(!serverAdapter) throw tr("Server not connected");

        QAbstractItemModel *dataModel = ModelManager::instance()->model(descriptorUid, m_ExperimentUid);
        m_CurrentView->setModel(dataModel);

        /* Reset the view filter for this model */
        ui->grpViewFilter->setChecked(false);
        ui->txtViewFilter->setText(QString());
        ui->cmbViewFilterColumn->clear();
        for(int i = 0; i < dataModel->columnCount(); i++) {
            ui->cmbViewFilterColumn->addItem(dataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to fetch experiemnt model: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to fetch experiement model."), NotificationWidget::Critical);
    }
}


void ExperimentWidget::on_btnAddModel_clicked()
{
    try {

        ModelManagerDialog models;
        models.exec();

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to open model descriptor dialog: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to open model descriptor dialog."), NotificationWidget::Critical);
    }
}

void ExperimentWidget::on_txtViewFilter_textChanged(const QString &text)
{
    Q_UNUSED(text)

    try {

        TreeView *treeView = qobject_cast<TreeView *>(m_CurrentView);
        if(treeView) {
            treeView->setFilter(ui->txtViewFilter->text());
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to change filter text: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to change filter text."), NotificationWidget::Critical);
    }
}

void ExperimentWidget::on_cmbViewFilterColumn_currentIndexChanged(int index)
{
    try {

        TreeView *treeView = qobject_cast<TreeView *>(m_CurrentView);
        if(treeView) {
            treeView->setFilterColumn(index);
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to change filter column: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to change filter column."), NotificationWidget::Critical);
    }
}

void ExperimentWidget::on_grpViewFilter_toggled(bool on)
{
    Q_UNUSED(on)

    try {

        ui->txtViewFilter->setText(QString());

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to open filtering: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to open filtering."), NotificationWidget::Critical);
    }
}





} // namespace OpenSpeedShop
} // namespace Plugins
