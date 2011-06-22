#include "OpenSpeedShopWidget.h"
#include "ui_OpenSpeedShopWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

OpenSpeedShopWidget::OpenSpeedShopWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::OpenSpeedShopWidget)
{
    ui->setupUi(this);
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeExperiment(int)));

    /* Set up the stylesheet for periods when we have tabs */
    m_StyleSheet = styleSheet();

    setWindowTitle(QString("Open|SpeedShop%1").arg(QChar(0x2122))); //Trademark
    setWindowIcon(QIcon(":/OpenSpeedShop/app.png"));

    Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
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

            m_CloseExperiment = new QAction(tr("Close Experiment"), this);
            m_CloseExperiment->setToolTip(tr("Close the current Open|SpeedShop experiment"));
            connect(m_CloseExperiment, SIGNAL(triggered()), this, SLOT(closeExperiment()));
            action->menu()->addAction(m_CloseExperiment);
            m_CloseExperiment->setEnabled(false);
        }
    }
}

OpenSpeedShopWidget::~OpenSpeedShopWidget()
{
    delete ui;
}

void OpenSpeedShopWidget::createExperiment()
{
    try {

        Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
        mainWindow.setCurrentCentralWidget(this);

        ExperimentWidget *experimentWidget = new ExperimentWidget(this);
        experimentWidget->create();
        int index = addTab(experimentWidget, experimentWidget->windowTitle());
        setCurrentIndex(index);

        connect(experimentWidget, SIGNAL(windowTitleChanged()), this, SLOT(tabTitleChanged()));

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to create experiment: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to create experiment."), NotificationWidget::Critical);
    }
}

void OpenSpeedShopWidget::loadExperiment()
{
    try {

        Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
        mainWindow.setCurrentCentralWidget(this);

        ExperimentWidget *experimentWidget = new ExperimentWidget(this);
        experimentWidget->load();
        int index = addTab(experimentWidget, experimentWidget->windowTitle());
        setCurrentIndex(index);

        connect(experimentWidget, SIGNAL(windowTitleChanged()), this, SLOT(tabTitleChanged()));

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to load experiment: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to load experiment."), NotificationWidget::Critical);
    }
}

void OpenSpeedShopWidget::closeExperiment(int index)
{
    try {

        if(index < 0) {
            index = currentIndex();
        }

        ExperimentWidget *experimentWidget = qobject_cast<ExperimentWidget *>(widget(index));
        if(experimentWidget->close()) {
            removeTab(index);
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to load experiment: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to load experiment."), NotificationWidget::Critical);
    }
}

void OpenSpeedShopWidget::tabInserted(int index)
{
    Q_UNUSED(index)

    if(this->count() < 2) {
        this->tabBar()->hide();
    } else {
        this->tabBar()->show();
    }

    /* Set the stylesheet to nothing if we have tabs, otherwise we'll run into display issues */
    if(count() <= 0) {
        setStyleSheet(m_StyleSheet);
    } else {
        setStyleSheet(QString());
    }

    m_CloseExperiment->setEnabled(count());
}

void OpenSpeedShopWidget::tabRemoved(int index)
{
    Q_UNUSED(index)

    if(this->count() < 2) {
        this->tabBar()->hide();
    } else {
        this->tabBar()->show();
    }

    /* Set the stylesheet to nothing if we have tabs, otherwise we'll run into display issues */
    if(count() <= 0) {
        setStyleSheet(m_StyleSheet);
    } else {
        setStyleSheet(QString());
    }

    m_CloseExperiment->setEnabled(count());
}

void OpenSpeedShopWidget::tabTitleChanged()
{
    try {

        QWidget *widget = qobject_cast<QWidget *>(QObject::sender());
        if(widget) {
            setTabText(indexOf(widget), widget->windowTitle());
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to change tab name: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to change tab name."), NotificationWidget::Critical);
    }
}



} // namespace OpenSpeedShop
} // namespace Plugins
