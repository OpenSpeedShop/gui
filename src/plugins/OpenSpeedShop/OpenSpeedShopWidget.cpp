#include "OpenSpeedShopWidget.h"
#include "ui_OpenSpeedShopWidget.h"

#include <QDebug>

#include <ActionManager/ActionManager.h>

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

    using namespace Core::ActionManager;
    ActionManager &actionManager = ActionManager::instance();

    m_Context = actionManager.createContext();

    MenuPath menuPath("File");
    actionManager.createMenuPath(menuPath);

    m_CreateExperiment = new QAction(tr("New O|SS Experiment"), this);
    m_CreateExperiment->setToolTip(tr("Create a new Open|SpeedShop experiment"));
    m_CreateExperiment->setIcon(QIcon(":/OpenSpeedShop/app.png"));
    m_CreateExperiment->setIconVisibleInMenu(true);
    m_CreateExperiment->setEnabled(false);      //FIXME: When we're set up to create, enable this
    connect(m_CreateExperiment, SIGNAL(triggered()), this, SLOT(createExperiment()));

    m_LoadExperiment = new QAction(tr("Load O|SS Experiment"), this);
    m_LoadExperiment->setToolTip(tr("Load an Open|SpeedShop experiment"));
    m_LoadExperiment->setIcon(QIcon(":/OpenSpeedShop/app.png"));
    m_LoadExperiment->setIconVisibleInMenu(true);
    connect(m_LoadExperiment, SIGNAL(triggered()), this, SLOT(loadExperiment()));

    m_CloseExperiment = new QAction(tr("Close O|SS Experiment"), this);
    m_CloseExperiment->setToolTip(tr("Close the current Open|SpeedShop experiment"));
    m_CloseExperiment->setIcon(QIcon(":/OpenSpeedShop/app.png"));
    m_CloseExperiment->setIconVisibleInMenu(true);
    m_CloseExperiment->setEnabled(false);
    connect(m_CloseExperiment, SIGNAL(triggered()), this, SLOT(closeExperiment()));

    actionManager.registerAction(m_Context, menuPath, m_CreateExperiment);
    actionManager.registerAction(m_Context, menuPath, m_LoadExperiment);
    actionManager.registerAction(m_Context, menuPath, m_CloseExperiment);

    connect(this, SIGNAL(active()), m_Context, SLOT(show()));
}

OpenSpeedShopWidget::~OpenSpeedShopWidget()
{
    delete ui;
}

void OpenSpeedShopWidget::createExperiment()
{
    try {

        emit active();

        ExperimentWidget *experimentWidget = new ExperimentWidget(this);
        experimentWidget->create();
        int index = addTab(experimentWidget, experimentWidget->windowTitle());
        setCurrentIndex(index);

        connect(experimentWidget, SIGNAL(windowTitleChanged()), this, SLOT(tabTitleChanged()));

    } catch(QString err) {
        qCritical() << tr("Failed to create experiment: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to create experiment.");
    }
}

void OpenSpeedShopWidget::loadExperiment()
{
    ExperimentWidget *experimentWidget = NULL;

    try {

        emit active();

        experimentWidget = new ExperimentWidget(this);
        if(experimentWidget->load()) {
            int index = addTab(experimentWidget, experimentWidget->windowTitle());
            setCurrentIndex(index);

            connect(experimentWidget, SIGNAL(windowTitleChanged()), this, SLOT(tabTitleChanged()));
            return;
        }

    } catch(QString err) {
        qCritical() << tr("Failed to load experiment: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to load experiment.");
    }

    // User canceled, or critical error, delete it!
    experimentWidget->setParent(NULL);
    experimentWidget->deleteLater();
}

void OpenSpeedShopWidget::closeExperiment(int index)
{
    try {

        if(index < 0) {
            index = currentIndex();
        }

        QWidget *widget = this->widget(index);
        if(widget && widget->close()) {
            removeTab(index);
            widget->deleteLater();
        }

    } catch(QString err) {
        qCritical() << tr("Failed to close experiment: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to close experiment: %1");
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

    QTabWidget::tabRemoved(index);
}

void OpenSpeedShopWidget::tabTitleChanged()
{
    try {

        QWidget *widget = qobject_cast<QWidget *>(QObject::sender());
        if(widget) {
            setTabText(indexOf(widget), widget->windowTitle());
        }

    } catch(QString err) {
        qCritical() << tr("Failed to change tab name: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to change tab name: %1");
    }
}

void OpenSpeedShopWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    emit active();

    QTabWidget::showEvent(event);
}

void OpenSpeedShopWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    m_Context->setVisible(false);

    QTabWidget::hideEvent(event);
}



} // namespace OpenSpeedShop
} // namespace Plugins
