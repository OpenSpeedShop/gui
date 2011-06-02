#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <QDomDocument>
#include <QInputDialog>

#include <MainWindow/MainWindow.h>
#include <ConnectionManager/ConnectionManager.h>
#include <ConnectionManager/IAdapter.h>

#include "ModelManager/ModelManager.h"
#include "ModelManager/ModelManagerDialog.h"
#include "ModelManager/ModelDescriptorListWidget.h"

#include "ViewManager/ViewManager.h"
#include "ViewManager/IViewFilterable.h"

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace OpenSpeedShop {

ExperimentWidget::ExperimentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentWidget)
{
    m_CurrentView = NULL;
    m_CurrentModel = NULL;

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->grpViewFilter->hide();

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

    //TODO: Create a real way for the user to interact with the remote filesystem
    bool okay;
    QString filePath = QInputDialog::getText(this,
                                             tr("Load experiment"),
                                             tr("Filepath"),
                                             QLineEdit::Normal,
//                                             "/home/dane/smg2000-pcsamp.openss",
                                             "/home/dane/smg2000-io.openss",
                                             &okay);
    if(!okay || filePath.isEmpty()) throw tr("Could not load experiemnt; invalid filepath.");

    setWindowFilePath(filePath);
    setWindowTitle(filePath);
    emit windowTitleChanged();
    m_ExperimentUid = serverAdapter->waitRestore(filePath);

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

        m_CurrentModel = ModelManager::instance()->model(descriptorUid, m_ExperimentUid);

        /* Reset the view filter for this model */
        ui->txtViewFilter->setText(QString());
        ui->cmbViewFilterColumn->clear();
        for(int i = 0; i < m_CurrentModel->columnCount(); i++) {
            ui->cmbViewFilterColumn->addItem(m_CurrentModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
        }

        /* Reset the view list */
        ui->cmbViews->clear();
        ui->cmbViews->addItems(ViewManager::instance()->viewNames(m_CurrentModel));
        ui->cmbViews->setCurrentIndex(0);

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to fetch experiemnt model: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to fetch experiement model."), NotificationWidget::Critical);
    }
}

void ExperimentWidget::on_cmbViews_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    try {

        if(m_CurrentView) {
            ui->grpView->layout()->removeWidget(m_CurrentView);
            m_CurrentView->deleteLater();
        }

        if(!ui->cmbViews->currentText().isEmpty()) {
            m_CurrentView = ViewManager::instance()->viewWidget(ui->cmbViews->currentText(), m_CurrentModel);
            ui->grpView->layout()->addWidget(m_CurrentView);

            IViewFilterable *viewFilterable = qobject_cast<IViewFilterable *>(m_CurrentView);
            if(viewFilterable) {
                ui->grpViewFilter->show();
            } else {
                ui->grpViewFilter->hide();
            }

        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to fetch view widget: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to fetch view widget."), NotificationWidget::Critical);
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

        IViewFilterable *viewFilterable = qobject_cast<IViewFilterable *>(m_CurrentView);
        if(viewFilterable) {
            viewFilterable->setViewFilter(ui->txtViewFilter->text());
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

        IViewFilterable *viewFilterable = qobject_cast<IViewFilterable *>(m_CurrentView);
        if(viewFilterable) {
            viewFilterable->setViewFilterColumn(index);
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to change filter column: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance()->notify(tr("Failed to change filter column."), NotificationWidget::Critical);
    }
}


} // namespace OpenSpeedShop
} // namespace Plugins
