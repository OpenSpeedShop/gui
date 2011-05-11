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

    qDebug() << serverAdapter->waitVersion();

    qDebug() << serverAdapter->waitExperimentTypes();
    qint64 expId = serverAdapter->waitRestore("/home/dane/smg2000-io.openss");
    serverAdapter->waitExperimentView(expId);

    serverAdapter->waitExit();

}


} // namespace OpenSpeedShop
} // namespace Plugins
