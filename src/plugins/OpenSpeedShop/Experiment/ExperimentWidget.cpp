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

#include "RemoteFileSystem/RemoteFileDialog.h"


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

    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    QStringList experimentTypes = adapter->waitExperimentTypes();
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

#ifdef QT_DEBUG
//    QString fileName = QFileDialog::getOpenFileName(this,
//                                                    tr("Open Open|SpeedShop experiment"),
//                                                    "openss::///",
//                                                    tr("Open|SpeedShop Files (*.openss)"));
//    qDebug() << __FILE__ << __LINE__ << "FileName:" << fileName;

    RemoteFileDialog dlg(this);
    dlg.exec();

#endif


    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

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
    m_ExperimentUid = adapter->waitRestore(filePath);

    ui->txtDatabasePath->setText(adapter->waitExperimentDatabase(m_ExperimentUid));
    ui->txtExecutablePath->setText(adapter->waitExperimentExecutable(m_ExperimentUid));
    ui->txtCommand->setText(adapter->waitExperimentAppCommand(m_ExperimentUid));

    QString experimentType = adapter->waitExperimentType(m_ExperimentUid);
    ui->cmbExperimentTypes->setCurrentIndex(ui->cmbExperimentTypes->findText(experimentType));

    loadModelDescriptors(experimentType);

//    qDebug() << "parameters" << adapter->waitExperimentParameterValues(expId);

//    qDebug() << "hosts" << adapter->waitExperimentHosts(expId);
//    qDebug() << "ranks" << adapter->waitExperimentRanks(expId);
//    qDebug() << "pids" << adapter->waitExperimentPids(expId);

//    qDebug() << "sources" << adapter->waitExperimentSourceFiles(expId);
//    qDebug() << "objects" << adapter->waitExperimentObjectFiles(expId);
//    qDebug() << "threads" << adapter->waitExperimentThreads(expId);

//    qDebug() << "Processes:";
//    QList<IAdapter::Process> processes = adapter->experimentProcesses(expId);
//    foreach(IAdapter::Process process, processes) {
//        qDebug() << "\t" << process.host << process.processId << process.threadId << process.rank << process.executable;
//    }

}

void ExperimentWidget::loadModelDescriptors(QString experimentType)
{
    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    //TODO: Remove any previous model descriptor lists.

    /* Create and initialize the model descriptor list. */
    ModelDescriptorListWidget *listWidget = ModelManager::instance().createDescriptorListWidget(ui->modelListViewParent);
    listWidget->setHeaderHidden(true);
    listWidget->setExperimentType(experimentType);
    ui->modelListViewParent->layout()->addWidget(listWidget);
    connect(listWidget, SIGNAL(currentDescriptorChanged(QUuid)), this, SLOT(getModel(QUuid)));
}

void ExperimentWidget::getModel(QUuid descriptorUid)
{
    try {

        m_CurrentModel = ModelManager::instance().model(descriptorUid, m_ExperimentUid);

        /* Reset the view filter for this model */
        ui->txtViewFilter->setText(QString());
        ui->cmbViewFilterColumn->clear();
        for(int i = 0; i < m_CurrentModel->columnCount(); i++) {
            ui->cmbViewFilterColumn->addItem(m_CurrentModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
        }

        /* Reset the view list */
        ui->cmbViews->clear();
        ui->cmbViews->addItems(ViewManager::instance().viewNames(m_CurrentModel));
        ui->cmbViews->setCurrentIndex(0);

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to fetch experiemnt model: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to fetch experiement model."), NotificationWidget::Critical);
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
            m_CurrentView = ViewManager::instance().viewWidget(ui->cmbViews->currentText(), m_CurrentModel);
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
        MainWindow::instance().notify(tr("Failed to fetch view widget: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to fetch view widget."), NotificationWidget::Critical);
    }
}


void ExperimentWidget::on_btnAddModel_clicked()
{
    try {

        ModelManagerDialog models;
        models.exec();

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open model descriptor dialog: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open model descriptor dialog."), NotificationWidget::Critical);
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
        MainWindow::instance().notify(tr("Failed to change filter text: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to change filter text."), NotificationWidget::Critical);
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
        MainWindow::instance().notify(tr("Failed to change filter column: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to change filter column."), NotificationWidget::Critical);
    }
}


} // namespace OpenSpeedShop
} // namespace Plugins
