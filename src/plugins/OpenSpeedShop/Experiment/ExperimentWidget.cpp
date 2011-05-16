#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <MainWindow/MainWindow.h>
#include "ConnectionManager/ConnectionManager.h"
#include "ConnectionManager/ServerAdapter.h"
#include "ConnectionManager/ServerCommand.h"
#include <QDomDocument>

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
//    qDebug() << serverAdapter->waitExperimentTypes();

    qint64 expId = serverAdapter->waitRestore("/home/dane/smg2000-pcsamp.openss");
    try {
    DataModel *dataModel = serverAdapter->waitExperimentView(expId);
//    qDebug() << dataModel->dumpModel();
    ui->treeView->setModel(dataModel);
    } catch(QString err) {
        using namespace Core::MainWindow;
        QString errorMessage = tr("An error occured while loading the experiment view: '%1'").arg(err);
        MainWindow::instance()->notify(errorMessage, NotificationWidget::Critical);
    }

//    serverAdapter->waitExit();

}


} // namespace OpenSpeedShop
} // namespace Plugins
