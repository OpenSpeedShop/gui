#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

#include <ConnectionManager/ConnectionManager.h>
#include <ConnectionManager/IAdapter.h>
#include <SettingManager/SettingManager.h>

#include <ModelManager/FilterDescriptor.h>
#include <ModelManager/ModelManager.h>
#include <ModelManager/ModelManagerDialog.h>
#include <ModelManager/ModelDescriptorListWidget.h>

#include <ViewManager/ViewManager.h>
#include <ViewManager/IViewFilterable.h>

#include <RemoteFileSystem/RemoteFileDialog.h>
#include <RemoteFileSystem/PathRewriter.h>

#include <NotificationManager/NotificationManager.h>

#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QSortFilterProxyModel>

#include <QDebug>

namespace Plugins {
namespace OpenSpeedShop {

ExperimentWidget::ExperimentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentWidget),
    m_FetchingNotify(NULL)
{
    m_FetchingTimer = 0;
    m_FetchingTimerLong = 0;

    m_CurrentView = NULL;
    m_CurrentModel = NULL;

    m_FilterDirty = false;

    m_lstSourceContextMenuRow = -1;

    ui->setupUi(this);

#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
    ui->txtViewFilter->setPlaceholderText(tr("regular expression"));
#endif

    ui->tabWidget->setCurrentIndex(0);
    ui->grpViewFilter->hide();


    // Set up the proxy model for node filtering and sorting
    m_ProxyModelProcesses = new QSortFilterProxyModel();
    m_ProxyModelProcesses->setFilterKeyColumn(0);
    m_ProxyModelProcesses->setFilterRole(Qt::DisplayRole);
    m_ProxyModelProcesses->setDynamicSortFilter(true);
    m_ProxyModelProcesses->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_ProxyModelProcesses->setSortRole(Qt::DisplayRole);
    m_ProxyModelProcesses->setSourceModel(new QStandardItemModel());
    ui->trvProcesses->setModel(m_ProxyModelProcesses);

    connect(ui->trvProcesses->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(trvProcesses_selectionChanged()));


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

    if(m_FetchingNotify) {
        m_FetchingNotify->close();
        m_FetchingNotify->deleteLater();
        m_FetchingNotify = NULL;
    }
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
    settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");

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
    settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");

    settingManager.setValue("splitSource/state", ui->splitSource->saveState());

    settingManager.endGroup();
}


void ExperimentWidget::create()
{
}

bool ExperimentWidget::load()
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
    settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");
    QString filePath = settingManager.value("defaultExperimentPath", QLatin1String("/")).toString();
    settingManager.endGroup();

    bool defaultPath = (filePath == QLatin1String("/"));
    QString absolutePath;

    if(adapter->useLocalFileDialog()) {
        filePath = QFileDialog::getOpenFileName(this, tr("Open File"), filePath, "*.openss");
        if(filePath.isEmpty()) {
            return false;
        }
        absolutePath = QFileInfo(filePath).absolutePath();
    } else {
        RemoteFileDialog dlg(this);
        dlg.setPath(filePath);
        dlg.setFilter("*.openss");
        if(!dlg.exec()) {
            return false;
//            throw tr("Could not load experiment: canceled by user.");
        }

        filePath = dlg.selectedFilePath();
        absolutePath = dlg.path();
    }

    if(filePath.isEmpty()) {
        throw tr("Could not load experiemnt: invalid filepath.");
    }

    /* User convenience.  If the path wasn't set in the settings, we'll set it for the user the first time. */
    if(defaultPath) {
        settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");
        settingManager.setValue("defaultExperimentPath", absolutePath);
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

    return true;
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
        bool keepGoing = (sourcePaths.count() > 1);    // There has to be more than one in the list!
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

        /* See if we can get the path from the list of previously used paths */
        m_CommonPath = QLatin1Char('/') + commonPath.join(QLatin1String("/"));
        QString newPath = PathRewriter::instance().rewrite(m_CommonPath);

        /* Make it available to the user in the list */
        ui->txtSourcePath->setText(newPath);
        foreach(QStringList path, sourcePaths) {
            QListWidgetItem *item = new QListWidgetItem(ui->lstSource);
            item->setText(path.join(QLatin1String("/")));
            ui->lstSource->addItem(item);
        }
        refreshSourceIcon();
    }
}

void ExperimentWidget::refreshSourceIcon(int row)
{
    if(row < 0) {
        for(int i = 0; i < ui->lstSource->count(); ++i) {
            refreshSourceIcon(i);
        }
    } else {
        QListWidgetItem *item = ui->lstSource->item(row);

        int rewriteType = PathRewriter::instance().hasRewrite(sourceFilePath(row, false));
        if(rewriteType == 1) {
            item->setIcon(QIcon(":/OpenSpeedShop/SourceFile/sourcefile-linked.svg"));
            item->setToolTip(tr("Path specified manually: \"%1\"").arg(sourceFilePath(row)));
        } else if(rewriteType == 2) {
            item->setIcon(QIcon(":/OpenSpeedShop/SourceFile/sourcefile-pattern.svg"));
            item->setToolTip(tr("Path rewrite pattern replaced: \"%1\"").arg(sourceFilePath(row)));
        } else {
            item->setIcon(QIcon(":/OpenSpeedShop/SourceFile/sourcefile.svg"));
        }
    }
}


/*! Updates the node list and process tree views with data from adapter
    \sa ExperimentWidget::on_trvNodeList_selectionChanged()
 */
void ExperimentWidget::refreshProcessTree()
{
    IAdapter *adapter = ConnectionManager::instance().askAdapter();
    if(!adapter) throw tr("Server not connected");

    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_ProxyModelProcesses->sourceModel());
    Q_ASSERT(model);

    model->clear();

    QStringList headers;
    headers << tr("Host") << tr("Rank") << tr("Process ID") << tr("Thread ID");
    model->setColumnCount(headers.count());
    for(int i = 0; i < headers.count(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, headers.at(i), Qt::DisplayRole);
    }

    QStringList nodes;

    QList<IAdapter::Process> processes = adapter->waitExperimentProcesses(m_ExperimentUid);
    foreach(IAdapter::Process process, processes) {
        QList<QStandardItem *> items;
        QStandardItem *item = new QStandardItem();
        item->setData(process.host, Qt::DisplayRole);
        items.append(item);

        item = new QStandardItem();
        item->setData(process.rank, Qt::DisplayRole);
        items.append(item);

        item = new QStandardItem();
        item->setData(process.processId, Qt::DisplayRole);
        item->setData(process.executable, Qt::ToolTipRole);
        items.append(item);

        item = new QStandardItem();
        item->setData(process.threadId, Qt::DisplayRole);
        item->setData(process.executable, Qt::ToolTipRole);
        items.append(item);

        model->invisibleRootItem()->appendRow(items);

        nodes << process.host;
    }

    ui->trvNodeList->setNodes(nodes.join(","));


    for(int i=0; i < model->columnCount(); i++) {
        ui->trvProcesses->resizeColumnToContents(i);
    }

    m_ProxyModelProcesses->sort(1);
    m_ProxyModelProcesses->sort(0);
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

    QApplication::setOverrideCursor(Qt::BusyCursor);
    this->setDisabled(true);

    try {

        // Clear the model dependent stuff
        ui->txtViewFilter->setText(QString());
        ui->cmbViewFilterColumn->clear();
        ui->cmbViews->clear();

        if(m_CurrentView) {
            m_CurrentView->hide();
            ui->grpView->layout()->removeWidget(m_CurrentView);
            disconnect(m_CurrentView, SIGNAL(activated(QModelIndex)), this, SLOT(viewItemActivated(QModelIndex)));
            m_CurrentView->deleteLater();
            m_CurrentView = NULL;
        }

        // Start the timer so we can notify the user of a long process, only if it takes a long time
        m_FetchingTimer = startTimer(1500);

        m_CurrentModel = ModelManager::instance().model(descriptorUid, m_ExperimentUid, getFilter());
        m_CurrentDescriptorUid = descriptorUid;
        m_FilterDirty = false;

        // Stop the timers; we're done.
        killTimer(m_FetchingTimer);
        m_FetchingTimer = 0;
        killTimer(m_FetchingTimerLong);
        m_FetchingTimerLong = 0;

        if(m_FetchingNotify) {
            m_FetchingNotify->close();
            m_FetchingNotify->deleteLater();
            m_FetchingNotify = NULL;
        }


        if(!m_CurrentModel) {
            // Waiting on it to be fetched from the server
            return;
        }

        // Write the CLI command text directly to the notification window
        Core::NotificationManager::NotificationManager::instance().writeToLogFile(0, m_CurrentModel->property("CommandText").toString());

        /* Let the source view know about the change */
        ui->txtSource->setModel(m_CurrentModel);

        if(!m_CurrentModel) {
            m_CurrentDescriptorUid = QUuid();
            return;
        }

        /* Reset the view filter for this model */
        for(int i = 0; i < m_CurrentModel->columnCount(); i++) {
            ui->cmbViewFilterColumn->addItem(m_CurrentModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
        }

        /* Reset the view list */
        QStringList viewNameList = Core::ViewManager::ViewManager::instance().viewNames(m_CurrentModel);

        // Remove the view that the Open|SpeedShop View replaced
        viewNameList.removeAll("Table View");

        ui->cmbViews->addItems(viewNameList);

        ui->cmbViews->setCurrentIndex(ui->cmbViews->findText("Open|SpeedShop View", Qt::MatchExactly));


    } catch(QString err) {
        qCritical() << tr("Failed to fetch experiemnt model: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to fetch experiement model.");
    }

    this->setDisabled(false);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void ExperimentWidget::on_tabWidget_currentChanged(int index)
{
    if(m_FilterDirty && ui->tabWidget->tabText(index).compare(tr("Views")) == 0) {
        getModel(m_CurrentDescriptorUid);
    }
}

/*! Filter the process list based on the node list selection
    \sa ExperimentWidget::refreshProcessTree()
 */
void ExperimentWidget::on_trvNodeList_selectionChanged()
{
    QString nodeListFilter = ui->trvNodeList->selectedNodes(true);
    nodeListFilter = nodeListFilter.replace(',','|');    // Cludgy, but it works

    if(!nodeListFilter.isEmpty()) {
        nodeListFilter = nodeListFilter.prepend("^(").append(")$");
    }

    ui->trvProcesses->selectionModel()->clearSelection();

    m_ProxyModelProcesses->setFilterRegExp(nodeListFilter);
    m_ProxyModelProcesses->sort(1);
    m_ProxyModelProcesses->sort(0);
    for(int i=0; i < m_ProxyModelProcesses->columnCount(); i++) {
        ui->trvProcesses->resizeColumnToContents(i);
    }

    // Update the view & filter!
    m_FilterDirty = true;
}

void ExperimentWidget::trvProcesses_selectionChanged()
{
    // Update the view & filter!
    m_FilterDirty = true;
}

FilterDescriptor ExperimentWidget::getFilter() const
{
    FilterDescriptor retval;

    QStringList selectedNodes = ui->trvNodeList->selectedNodes(true).split(",");
    QModelIndexList selectedProcs = ui->trvProcesses->selectionModel()->selectedRows();

    bool allNodes = ((ui->trvNodeList->nodeCount() == selectedNodes.count()) || (selectedNodes.count() == 0));
    bool allProcs = ((ui->trvProcesses->model()->rowCount() == selectedProcs.count()) || (selectedProcs.count() == 0));

    if(!allProcs) {
        foreach(QModelIndex index, selectedProcs) {
            QModelIndex rankIndex = index.sibling(index.row(), 1);
            retval.insertRank(rankIndex.data(Qt::DisplayRole).toString());
        }
    }

    if(!allNodes && retval.isEmpty()) {
        // Grab ranks associated with each selected host, and insert them using retval.insertRank()
        for(int i = 0; i < m_ProxyModelProcesses->rowCount(); ++i) {
            retval.insertRank(m_ProxyModelProcesses->index(i, 1).data(Qt::DisplayRole).toString());
        }
    }

    return retval;
}

void ExperimentWidget::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();

    if(timerId == m_FetchingTimer) {
        if(m_FetchingNotify) {
            m_FetchingNotify->close();
            m_FetchingNotify->deleteLater();
            m_FetchingNotify = NULL;
        }

        using namespace Core::NotificationManager;
        m_FetchingNotify = NotificationManager::instance().notify(tr("Compiling data."), NotificationWidget::Loading);

        killTimer(m_FetchingTimer);
        m_FetchingTimer = 0;

        m_FetchingTimerLong = startTimer(5000);

        event->accept();
        return;

    } else if(timerId == m_FetchingTimerLong) {
        if(m_FetchingNotify) {
            m_FetchingNotify->setText(tr("Still compiling data. Thank you for your patience."));
        }

        m_FetchingTimerLong = 0;

        event->accept();
        return;

    }

    QWidget::timerEvent(event);
}



void ExperimentWidget::on_cmbViews_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    try {

        if(m_CurrentView) {
            m_CurrentView->hide();
            ui->grpView->layout()->removeWidget(m_CurrentView);
            disconnect(m_CurrentView, SIGNAL(activated(QModelIndex)), this, SLOT(viewItemActivated(QModelIndex)));
            m_CurrentView->deleteLater();
        }

        if(!ui->cmbViews->currentText().isEmpty()) {
            m_CurrentView = Core::ViewManager::ViewManager::instance().viewWidget(ui->cmbViews->currentText(), m_CurrentModel);
            ui->grpView->layout()->addWidget(m_CurrentView);

            connect(m_CurrentView, SIGNAL(activated(QModelIndex)), this, SLOT(viewItemActivated(QModelIndex)));

            Core::ViewManager::IViewFilterable *viewFilterable = qobject_cast<Core::ViewManager::IViewFilterable *>(m_CurrentView);
            if(viewFilterable) {
                ui->grpViewFilter->show();
            } else {
                ui->grpViewFilter->hide();
            }
        }

    } catch(QString err) {
        qCritical() << tr("Failed to fetch view widget: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to fetch view widget.");
    }
}


void ExperimentWidget::on_btnAddModel_clicked()
{
    try {

        ModelManagerDialog models;
        models.exec();

    } catch(QString err) {
        qCritical() << tr("Failed to open model descriptor dialog: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to open model descriptor dialog.");
    }
}

void ExperimentWidget::on_txtViewFilter_textChanged(const QString &text)
{
    Q_UNUSED(text)

    try {

        Core::ViewManager::IViewFilterable *viewFilterable = qobject_cast<Core::ViewManager::IViewFilterable *>(m_CurrentView);
        if(viewFilterable) {
            viewFilterable->setViewFilter(ui->txtViewFilter->text());
        }

    } catch(QString err) {
        qCritical() << tr("Failed to change filter text: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to change filter text.");
    }
}

void ExperimentWidget::on_cmbViewFilterColumn_currentIndexChanged(int index)
{
    try {

        Core::ViewManager::IViewFilterable *viewFilterable = qobject_cast<Core::ViewManager::IViewFilterable *>(m_CurrentView);
        if(viewFilterable) {
            viewFilterable->setViewFilterColumn(index);
        }

    } catch(QString err) {
        qCritical() << tr("Failed to change filter column: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to change filter column.");
    }
}

QString ExperimentWidget::sourceFilePath(int row, bool rewrite)
{
    QListWidgetItem *item = ui->lstSource->item(row);
    if(item == NULL) {
        return QString();
    }

    QString fileName = item->text();

    // Recreate the original file path
    QString filePath = m_CommonPath;
    if(!filePath.endsWith(QLatin1Char('/'))) {
        filePath.append(QLatin1Char('/'));
    }
    filePath.append(fileName);

    // Are we looking for the orginal?
    if(!rewrite) {
        return filePath;
    }

    // See if it's rewritten
    if(PathRewriter::instance().hasRewrite(filePath)) {
        return PathRewriter::instance().rewrite(filePath);
    }

    // If not explicitly rewritten, see if we rewrote the common path
    filePath = ui->txtSourcePath->text();
    if(!filePath.endsWith(QLatin1Char('/'))) {
        filePath.append(QLatin1Char('/'));
    }
    filePath.append(fileName);
    return filePath;
}

QString ExperimentWidget::rewriteSourceFilePath(int row)
{
    QString filePath = sourceFilePath(row, false);

    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");
    QString startPath = settingManager.value("defaultSourcePath", QLatin1String("/")).toString();
    settingManager.endGroup();

    RemoteFileDialog dlg(this);
    dlg.setPath(startPath);
    dlg.setWindowTitle(tr("Locate replacement file for: %1").arg(filePath));
    if(!dlg.exec()) {
        return filePath;
    }

    QString newPath = dlg.selectedFilePath();
    PathRewriter::instance().setRewrite(filePath, newPath);
    refreshSourceIcon(row);
    return newPath;
}

void ExperimentWidget::on_lstSource_currentRowChanged(int row)
{
    ui->txtSource->clear();
    ui->txtSource->setFilePath(QString());

    try {

        QListWidgetItem *item = ui->lstSource->item(row);
        if(item == NULL) {
            return;
        }

        QString filePath(sourceFilePath(row));

        if(!m_SourceFileCache.contains(filePath)) {
            IAdapter *adapter = ConnectionManager::instance().askAdapter();
            if(!adapter) throw tr("Server not connected");

            if(adapter->waitFileExists(filePath)) {
                m_SourceFileCache.insert(filePath, adapter->waitCatFile(filePath));
            } else {
                QMessageBox msg(QMessageBox::Question, tr("File not found"),
                                tr("The file path, \"%1\", does not exist.\nWould you like to find a replacement manually?").arg(filePath),
                                QMessageBox::Yes|QMessageBox::No);

                if(msg.exec() == QMessageBox::Yes) {
                    filePath = rewriteSourceFilePath(row);
                    m_SourceFileCache.insert(filePath, adapter->waitCatFile(filePath));
                }
            }
        }

        ui->txtSource->setPlainText(m_SourceFileCache.value(filePath));
        ui->txtSource->setFilePath(sourceFilePath(row, false));

    } catch(QString err) {
        qCritical() << tr("Failed to open source file: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to open source file.");
    }
}

void ExperimentWidget::on_txtSourcePath_editingFinished()
{
    try {
        Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
        settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");
        QString filePath = settingManager.value("defaultSourcePath", QLatin1String("/")).toString();
        settingManager.endGroup();

        bool defaultPath = (filePath == QLatin1String("/"));

        if(filePath.isEmpty()) {
            filePath = QString(QLatin1Char('/'));
        }

        filePath = ui->txtSourcePath->text();
        PathRewriter::instance().setRewrite(m_CommonPath, filePath);

        /* User convenience.  If the path wasn't set in the settings, we'll set it for the user the first time. */
        if(defaultPath) {
            settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");
            settingManager.setValue("defaultSourcePath", filePath);
            settingManager.endGroup();
        }

        /* Update the source view */
        on_lstSource_currentRowChanged(ui->lstSource->currentRow());

    } catch(QString err) {
        qCritical() << tr("Failed to open source path: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to open source path.");
    }
}

void ExperimentWidget::on_btnSourcePath_clicked()
{
    try {
        Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
        settingManager.setGroup("Plugins/OpenSpeedShop/Experiment");
        QString filePath = settingManager.value("defaultSourcePath", QLatin1String("/")).toString();
        settingManager.endGroup();

        RemoteFileDialog dlg(this);
        dlg.setPath(filePath);
        if(!dlg.exec()) {
            return;
        }

        filePath = dlg.path();
        ui->txtSourcePath->setText(filePath);
        on_txtSourcePath_editingFinished();

    } catch(QString err) {
        qCritical() << tr("Failed to open source path from dialog: %1").arg(err);
    } catch(...) {
        qCritical() << tr("Failed to open source path from dialog.");
    }
}

void ExperimentWidget::viewItemActivated(QModelIndex index)
{
    QVariant indexData = index.data(Qt::DisplayRole);
    if(indexData.type() == QVariant::String) {
        QString text = indexData.toString();

        QRegExp statementPattern("[\\(]*(\\S+):(\\d+)", Qt::CaseInsensitive, QRegExp::RegExp2);

        int matchIndex = statementPattern.indexIn(text);
        if(matchIndex > -1) {
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

void ExperimentWidget::on_lstSource_customContextMenuRequested(const QPoint &position)
{
    QListWidgetItem *item = ui->lstSource->itemAt(position);

    if(item) {
        m_lstSourceContextMenuRow = ui->lstSource->row(item);

        QMenu contextMenu(tr("Context Menu"), ui->lstSource);

        QAction refresh(tr("Refresh file"), this);
        connect(&refresh, SIGNAL(triggered()), this, SLOT(refreshSourcePath()));
        contextMenu.addAction(&refresh);

        QAction rewrite(tr("Rewrite file path"), this);
        connect(&rewrite, SIGNAL(triggered()), this, SLOT(rewriteSourcePath()));
        contextMenu.addAction(&rewrite);

        QAction reset(tr("Reset file path"), this);
        connect(&reset, SIGNAL(triggered()), this, SLOT(resetSourcePath()));
        QString filePath = sourceFilePath(m_lstSourceContextMenuRow, false);
        reset.setEnabled(PathRewriter::instance().hasRewrite(filePath) == 1);
        contextMenu.addAction(&reset);

        contextMenu.exec(ui->lstSource->mapToGlobal(position));

        m_lstSourceContextMenuRow = -1;
    }
}


void ExperimentWidget::refreshSourcePath()
{
    if(m_lstSourceContextMenuRow < 0) {
        return;
    }

    QString filePath = sourceFilePath(m_lstSourceContextMenuRow);
    m_SourceFileCache.remove(filePath);
    refreshSourceIcon(m_lstSourceContextMenuRow);

    if(m_lstSourceContextMenuRow == ui->lstSource->currentRow()) {
        on_lstSource_currentRowChanged(m_lstSourceContextMenuRow);
    }
}

void ExperimentWidget::rewriteSourcePath()
{
    if(m_lstSourceContextMenuRow < 0) {
        return;
    }

    rewriteSourceFilePath(m_lstSourceContextMenuRow);

    if(m_lstSourceContextMenuRow == ui->lstSource->currentRow()) {
        on_lstSource_currentRowChanged(m_lstSourceContextMenuRow);
    }
}

void ExperimentWidget::resetSourcePath()
{
    if(m_lstSourceContextMenuRow < 0) {
        return;
    }

    QString filePath = sourceFilePath(m_lstSourceContextMenuRow, false);
    PathRewriter::instance().setRewrite(filePath, QString());
    refreshSourceIcon(m_lstSourceContextMenuRow);

    if(m_lstSourceContextMenuRow == ui->lstSource->currentRow()) {
        on_lstSource_currentRowChanged(m_lstSourceContextMenuRow);
    }
}


} // namespace OpenSpeedShop
} // namespace Plugins
