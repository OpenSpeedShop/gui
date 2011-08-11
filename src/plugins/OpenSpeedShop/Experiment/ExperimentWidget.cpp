#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <QDomDocument>
#include <QInputDialog>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

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

#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
    ui->txtViewFilter->setPlaceholderText(tr("regular expression"));
#endif

    ui->tabWidget->setCurrentIndex(0);
    ui->grpViewFilter->hide();

    // readSettings has to be called after we set up the presentation entirely
    readSettings();

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

void ExperimentWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    //TODO: Ask the user if they really, really.
    writeSettings();
}


void ExperimentWidget::readSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/Experiment");

    ui->splitSource->restoreState(settingManager.value("splitSource/state", ui->splitSource->saveState()).toByteArray());

    bool okay;

    /* Sample rate is set by the user in the main OpenSpeedShop plugin setting page */
    ui->txtSampleRate->setValue(settingManager.value("defaultSampleRate", ui->txtSampleRate->value()).toInt(&okay));
    if(!okay) { ui->txtSampleRate->setValue(60); }

    settingManager.endGroup();
}

void ExperimentWidget::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/Experiment");

    settingManager.setValue("splitSource/state", ui->splitSource->saveState());

    settingManager.endGroup();
}


void ExperimentWidget::create()
{
}

void ExperimentWidget::load()
{

    //! \todo Fix this cludge code to work with new additions, without having to synchronize manually
//    ui->tabProperties->setEnabled(false);     // This disables all controls, including the QLabels, which is undesireable
    ui->cmbExperimentTypes->setEnabled(false);
    ui->cmbExperimentModes->setEnabled(false);
    ui->txtSampleRate->setEnabled(false);
    ui->txtExecutablePath->setEnabled(false);
    ui->lstEnvironmentVariables->setEnabled(false);
    ui->txtDatabasePath->setEnabled(false);
    ui->txtCommand->setEnabled(false);
    ui->btnExecutablePath->setEnabled(false);
    ui->btnStart->setEnabled(false);
    ui->btnContinue->setEnabled(false);
    ui->btnPause->setEnabled(false);
    ui->btnRefresh->setEnabled(false);
    ui->btnStop->setEnabled(false);


    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop/Experiment");
    QString filePath = settingManager.value("defaultExperimentPath", QLatin1String("/")).toString();
    settingManager.endGroup();

    bool defaultPath = (filePath == QLatin1String("/"));

    RemoteFileDialog dlg(this);
    dlg.setPath(filePath);
    if(!dlg.exec()) {
        throw tr("Could not load experiment: canceled by user.");
    }

    filePath = dlg.selectedFilePath();
    if(filePath.isEmpty()) {        //TODO: Additional file path checking
        throw tr("Could not load experiemnt: invalid filepath.");
    }

    /* User convenience.  If the path wasn't set in the settings, we'll set it for the user the first time. */
    if(defaultPath) {
        settingManager.beginGroup("Plugins/OpenSpeedShop/Experiment");
        settingManager.setValue("defaultExperimentPath", dlg.path());
        settingManager.endGroup();
    }

    setWindowFilePath(filePath);
    setWindowTitle(filePath);
    emit windowTitleChanged();

    /* Have the server restore the database file */
    m_ExperimentUid = adapter->waitRestore(filePath);

    /* Set the properties to those of this experiment */
    ui->txtDatabasePath->setText(adapter->waitExperimentDatabase(m_ExperimentUid));
    ui->txtExecutablePath->setText(adapter->waitExperimentExecutable(m_ExperimentUid));
    ui->txtCommand->setText(adapter->waitExperimentAppCommand(m_ExperimentUid));

    /* Set the experiment type combo box to match this experiment */
    QString experimentType = adapter->waitExperimentType(m_ExperimentUid);
    ui->cmbExperimentTypes->setCurrentIndex(ui->cmbExperimentTypes->findText(experimentType));

    /* Load up the model descriptors for this experiment type */
    loadModelDescriptors(experimentType);

    /* Load the processes into the process tree */
    refreshProcessTree();

    /* Load the source paths into the source path list */
    refreshSourcePaths();

}

void ExperimentWidget::refreshSourcePaths()
{
    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    QStringList sources = adapter->waitExperimentSourceFiles(m_ExperimentUid);
    QList<QStringList> sourcePaths;
    foreach(QString source, sources) {
        QStringList path = source.split(QLatin1Char('/'), QString::SkipEmptyParts);
        if(!path.isEmpty()) {
            sourcePaths.append(path);
        }
    }

    /* Remove the common path names at the beginning */
    if(!sourcePaths.isEmpty()) {
        QStringList commonPath;
        bool keepGoing = true;
        while(keepGoing) {
            QString dir;
            foreach(QStringList path, sourcePaths) {
                if(path.isEmpty()) {
                    keepGoing = false;
                    break;
                }

                /* The first path is the one to compare with the others */
                if(path == sourcePaths.first()) {
                    dir = path.first();
                    continue;
                }

                /* If the first directory is different, quit iterating */
                if(dir != path.first()) {
                    keepGoing = false;
                    break;
                }

                /* If all of the first directories in the paths are equal, pop it onto the common path */
                if(path == sourcePaths.last()) {
                    commonPath.append(path.first());
                    for(int i=0; i<sourcePaths.count(); i++) {
                        sourcePaths[i].removeFirst();
                    }
                }
            }
        }

        /* Make it available to the user in the list */
        ui->txtSourcePath->setText(QLatin1Char('/') + commonPath.join(QLatin1String("/")));
        foreach(QStringList path, sourcePaths) {
            QListWidgetItem *item = new QListWidgetItem(ui->lstSource);
            item->setText(path.join(QLatin1String("/")));
            ui->lstSource->addItem(item);
        }
    }
}

void ExperimentWidget::refreshProcessTree()
{
    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    ui->trvProcesses->clear();
    QStringList headers;
    headers << tr("Host") << tr("Rank") << tr("Process ID") << tr("Thread ID") << tr("Executable");
    ui->trvProcesses->setHeaderLabels(headers);

    QList<IAdapter::Process> processes = adapter->waitExperimentProcesses(m_ExperimentUid);
    foreach(IAdapter::Process process, processes) {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->trvProcesses);
        item->setData(0, Qt::DisplayRole, process.host);
        item->setData(1, Qt::DisplayRole, process.rank);
        item->setData(2, Qt::DisplayRole, process.processId);
        item->setData(3, Qt::DisplayRole, process.threadId);
        item->setData(4, Qt::DisplayRole, process.executable);
        ui->trvProcesses->addTopLevelItem(item);
    }

    for(int i=0; i < ui->trvProcesses->columnCount(); i++) {
        ui->trvProcesses->resizeColumnToContents(i);
    }
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

    /* Select the default model descriptor */
    listWidget->selectDefault();
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
        m_DisableViewChange = true;
        ui->cmbViews->addItems(ViewManager::instance().viewNames(m_CurrentModel));
        m_DisableViewChange = false;
        ui->cmbViews->setCurrentIndex(ui->cmbViews->count() - 1);

        /* Let the source view know about the change */
        ui->txtSource->setModel(m_CurrentModel);

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
            disconnect(m_CurrentView, SIGNAL(activated(QModelIndex)), this, SLOT(viewItemActivated(QModelIndex)));
            m_CurrentView->deleteLater();
        }

        if(!m_DisableViewChange && !ui->cmbViews->currentText().isEmpty()) {
            m_CurrentView = ViewManager::instance().viewWidget(ui->cmbViews->currentText(), m_CurrentModel);
            ui->grpView->layout()->addWidget(m_CurrentView);

            connect(m_CurrentView, SIGNAL(activated(QModelIndex)), this, SLOT(viewItemActivated(QModelIndex)));

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

void ExperimentWidget::on_lstSource_currentRowChanged(int row)
{
    ui->txtSource->clear();

    try {

        QString filePath = ui->txtSourcePath->text();
        if(!filePath.endsWith(QLatin1Char('/'))) {
            filePath.append(QLatin1Char('/'));
        }

        QListWidgetItem *item = ui->lstSource->item(row);
        if(item == NULL) {
            return;
        }

        QString fileName = item->text();
        filePath.append(fileName);

        if(!m_SourceFileCache.contains(filePath)) {
            IAdapter *adapter = ConnectionManager::instance().askAdapter();
            if(!adapter) throw tr("Server not connected");
            m_SourceFileCache.insert(filePath, adapter->waitCatFile(filePath));
        }

        ui->txtSource->setPlainText(m_SourceFileCache.value(filePath));
        ui->txtSource->setFilePath(fileName);

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed open source file: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed open source file."), NotificationWidget::Critical);
    }
}

void ExperimentWidget::on_btnSourcePath_clicked()
{
    try {

        Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
        settingManager.beginGroup("Plugins/OpenSpeedShop/Experiment");
        QString filePath = settingManager.value("defaultSourcePath", QLatin1String("/")).toString();
        settingManager.endGroup();

        bool defaultPath = (filePath == QLatin1String("/"));

        if(filePath.isEmpty()) {
            filePath = QString(QLatin1Char('/'));
        }

        RemoteFileDialog dlg(this);
        dlg.setPath(filePath);
        if(!dlg.exec()) {
            return;
        }

        filePath = dlg.path();
        ui->txtSourcePath->setText(filePath);

        /* User convenience.  If the path wasn't set in the settings, we'll set it for the user the first time. */
        if(defaultPath) {
            settingManager.beginGroup("Plugins/OpenSpeedShop/Experiment");
            settingManager.setValue("defaultSourcePath", dlg.path());
            settingManager.endGroup();
        }

        /* Update the source view */
        on_lstSource_currentRowChanged(ui->lstSource->currentRow());

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed open source file: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed open source file."), NotificationWidget::Critical);
    }
}

void ExperimentWidget::viewItemActivated(QModelIndex index)
{
    QString type = index.data(Qt::UserRole + 1).toString();
    if(type == "Function" || type == "Statement") {
        QString text = index.data(Qt::DisplayRole).toString();

        QRegExp statementPattern;
        if(type == "Function") {
            statementPattern.setPattern("^.*\\((.*):([0-9]+)\\)$");
        } else if(type == "Statement") {
            statementPattern.setPattern("^(.*):([0-9]+)$");
        }

        if(statementPattern.exactMatch(text)) {
            QStringList filePathDirs = statementPattern.cap(1).split(QLatin1Char('/'), QString::SkipEmptyParts);

            QString filePath(filePathDirs.takeLast());
            QList<QListWidgetItem *> items;
            while(items.count() != 1 && filePathDirs.count() > 0) {
                items = ui->lstSource->findItems(filePath, Qt::MatchEndsWith|Qt::MatchCaseSensitive);
                filePath.prepend(filePathDirs.takeLast() + QLatin1Char('/'));
            }

            if(items.count() == 1) {
                ui->lstSource->setCurrentItem(items.first());

                bool okay;
                int lineNumber = statementPattern.cap(2).toInt(&okay);
                if(okay) {
                    ui->txtSource->setCurrentLineNumber(lineNumber);
                }

                ui->tabWidget->setCurrentWidget(ui->tabSource);
            }
        }
    }
}



} // namespace OpenSpeedShop
} // namespace Plugins
