#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <ConnectionManager/ConnectionManager.h>
#include <ConnectionManager/ServerCommand.h>

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

//    Plugins::ConnectionManager::ConnectionManager *connectionManager =
//            Plugins::ConnectionManager::ConnectionManager::instance();
//    connectionManager->setObjectName("test");


}


} // namespace OpenSpeedShop
} // namespace Plugins
