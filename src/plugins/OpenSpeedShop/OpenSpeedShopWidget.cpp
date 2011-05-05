#include "OpenSpeedShopWidget.h"
#include "ui_OpenSpeedShopWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

OpenSpeedShopWidget::OpenSpeedShopWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::OpenSpeedShopWidget)
{
    ui->setupUi(this);

    Core::MainWindow::MainWindow *mainWindow = Core::MainWindow::MainWindow::instance();

    foreach(QAction *action, mainWindow->menuBar()->actions()) {
        if(action->text() == tr("File")) {
            QAction *createExperiment = new QAction(tr("New Experiment"), this);
            createExperiment->setToolTip(tr("Create a new Open|SpeedShop experiment"));
            connect(createExperiment, SIGNAL(triggered()), this, SLOT(createExperiment()));
            action->menu()->addAction(createExperiment);

            //FIXME: When we're set up to create, enable this
            createExperiment->setEnabled(false);

            QAction *loadExperiment = new QAction(tr("Load Experiment"), this);
            loadExperiment->setToolTip(tr("Load an Open|SpeedShop experiment"));
            connect(loadExperiment, SIGNAL(triggered()), this, SLOT(loadExperiment()));
            action->menu()->addAction(loadExperiment);
        }
    }


}

OpenSpeedShopWidget::~OpenSpeedShopWidget()
{
    delete ui;
}

void OpenSpeedShopWidget::createExperiment()
{
    ExperimentWidget *experimentWidget = new ExperimentWidget(this);
    experimentWidget->create();
    int index = addTab(experimentWidget, experimentWidget->windowTitle());
    setCurrentIndex(index);
}

void OpenSpeedShopWidget::loadExperiment()
{
    ExperimentWidget *experimentWidget = new ExperimentWidget(this);
    experimentWidget->load();
    int index = addTab(experimentWidget, experimentWidget->windowTitle());
    setCurrentIndex(index);
}

} // namespace OpenSpeedShop
} // namespace Plugins
