#include "OpenSpeedShopWidget.h"
#include "ui_OpenSpeedShopWidget.h"

#include <MainWindow/MainWindow.h>

#include <Experiment/ExperimentWidget.h>

namespace Plugins {
namespace OpenSpeedShop {

OpenSpeedShopWidget::OpenSpeedShopWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::OpenSpeedShopWidget)
{
    m_CreateExperiment = NULL;
    m_LoadExperiment = NULL;
    m_CloseExperiment = NULL;

    ui->setupUi(this);
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeExperiment(int)));

    /* Set up the stylesheet for periods when we have tabs */
    m_StyleSheet = styleSheet();

    setWindowTitle(QString("Open|SpeedShop%1").arg(QChar(0x2122))); //Trademark
    setWindowIcon(QIcon(":/OpenSpeedShop/app.png"));

    Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("File")) {

            m_CreateExperiment = new QAction(tr("New O|SS Experiment"), this);
            m_CreateExperiment->setToolTip(tr("Create a new Open|SpeedShop experiment"));
            m_CreateExperiment->setIcon(QIcon(":/OpenSpeedShop/app.png"));
            m_CreateExperiment->setIconVisibleInMenu(true);
            m_CreateExperiment->setVisible(false);
            m_CreateExperiment->setProperty("oss_menuitem", QVariant(1));
            connect(m_CreateExperiment, SIGNAL(triggered()), this, SLOT(createExperiment()));

            //FIXME: When we're set up to create, enable this
            m_CreateExperiment->setEnabled(false);

            m_LoadExperiment = new QAction(tr("Load O|SS Experiment"), this);
            m_LoadExperiment->setToolTip(tr("Load an Open|SpeedShop experiment"));
            m_LoadExperiment->setIcon(QIcon(":/OpenSpeedShop/app.png"));
            m_LoadExperiment->setIconVisibleInMenu(true);
            m_LoadExperiment->setVisible(false);
            m_LoadExperiment->setProperty("oss_menuitem", QVariant(1));
            connect(m_LoadExperiment, SIGNAL(triggered()), this, SLOT(loadExperiment()));

            m_CloseExperiment = new QAction(tr("Close O|SS Experiment"), this);
            m_CloseExperiment->setToolTip(tr("Close the current Open|SpeedShop experiment"));
            m_CloseExperiment->setIcon(QIcon(":/OpenSpeedShop/app.png"));
            m_CloseExperiment->setIconVisibleInMenu(true);
            m_CloseExperiment->setEnabled(false);
            m_CloseExperiment->setVisible(false);
            m_CloseExperiment->setProperty("oss_menuitem", QVariant(1));
            connect(m_CloseExperiment, SIGNAL(triggered()), this, SLOT(closeExperiment()));

            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }

            if(before) {
                action->menu()->insertAction(before, m_CreateExperiment);
                action->menu()->insertAction(before, m_LoadExperiment);
                action->menu()->insertAction(before, m_CloseExperiment);
                action->menu()->insertSeparator(before)->setProperty("oss_menuitem", QVariant(1));
            } else {
                action->menu()->addAction(m_CreateExperiment);
                action->menu()->addAction(m_LoadExperiment);
                action->menu()->addAction(m_CloseExperiment);
                action->menu()->addSeparator()->setProperty("oss_menuitem", QVariant(1));
            }
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

        QWidget *widget = this->widget(index);
        if(widget->close()) {
            removeTab(index);
            widget->deleteLater();
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to close experiment: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to close experiment."), NotificationWidget::Critical);
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

void OpenSpeedShopWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("oss_menuitem").isValid()) {
            action->setVisible(true);
        }
    }
}

void OpenSpeedShopWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("oss_menuitem").isValid()) {
            action->setVisible(false);
        }
    }
}



} // namespace OpenSpeedShop
} // namespace Plugins
