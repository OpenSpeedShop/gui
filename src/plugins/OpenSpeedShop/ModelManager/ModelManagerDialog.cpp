#include "ModelManagerDialog.h"
#include "ui_ModelManagerDialog.h"

#include <SettingManager/SettingManager.h>
#include <MainWindow/MainWindow.h>

#include "ModelManager.h"
#include "ModelDescriptorWidget.h"
#include "ModelDescriptorListWidget.h"

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace OpenSpeedShop {

ModelManagerDialog::ModelManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelManagerDialog)
{
    ui->setupUi(this);

    // Create and add the descriptor list widget to the dialog
    ModelDescriptorListWidget *descriptorListWidget =
            ModelManager::instance().createDescriptorListWidget(ui->descriptorListParent);
    ui->descriptorListParent->layout()->addWidget(descriptorListWidget);
    ui->descriptorListParent->layout()->setMargin(0);
    connect(descriptorListWidget, SIGNAL(currentDescriptorChanged(QUuid)), this, SLOT(currentSelectionChanged(QUuid)));

    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop");

    restoreGeometry(settingManager.value("ModelManagerDialog/windowGeometry", saveGeometry()).toByteArray());
    ui->splitter->restoreGeometry(settingManager.value("ModelManagerDialog/splitterGeometry", ui->splitter->saveGeometry()).toByteArray());
    ui->splitter->restoreState(settingManager.value("ModelManagerDialog/splitterState", ui->splitter->saveState()).toByteArray());

    settingManager.endGroup();
}

ModelManagerDialog::~ModelManagerDialog()
{
    ModelManager::instance().exportDescriptors();

    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/OpenSpeedShop");
    settingManager.setValue("ModelManagerDialog/windowGeometry", saveGeometry());
    settingManager.setValue("ModelManagerDialog/splitterGeometry", ui->splitter->saveGeometry());
    settingManager.setValue("ModelManagerDialog/splitterState", ui->splitter->saveState());
    settingManager.endGroup();

    delete ui;
}

void ModelManagerDialog::currentSelectionChanged(const QUuid &current)
{
    // Let the current editor widget know that we're moving on (it should prompt the user to save)
    ModelDescriptorWidget *descriptorWidget = ui->descriptorParent->findChild<ModelDescriptorWidget *>();
    if(descriptorWidget) {
//        descriptorWidget->close();
        ui->descriptorParent->layout()->removeWidget(descriptorWidget);
        descriptorWidget->deleteLater();
    }

    if(!current.isNull()) {
        try {
            QWidget *descriptor = ModelManager::instance().createDescriptorWidget(current, this);
            if(!descriptor) {
                return;
            }

            ui->descriptorParent->layout()->addWidget(descriptor);
            ui->descriptorParent->layout()->setMargin(0);
        } catch(QString err) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Failed to open model descriptor: %1").arg(err), NotificationWidget::Critical);
        } catch(...) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Failed to open model descriptor."), NotificationWidget::Critical);
        }
    }
}

void ModelManagerDialog::on_btnCreate_clicked()
{
    QUuid descriptorId = ModelManager::instance().createDescriptor();

    // Select the new item in the view, which sets the editor to display it
    ModelDescriptorListWidget *descriptorListWidget = ui->descriptorListParent->findChild<ModelDescriptorListWidget *>();
    if(descriptorListWidget) {
        descriptorListWidget->selectRow(descriptorId);
    }
}

void ModelManagerDialog::on_btnRemove_clicked()
{
    // Remove selected model
    ModelDescriptorListWidget *descriptorListWidget = ui->descriptorListParent->findChild<ModelDescriptorListWidget *>();
    if(descriptorListWidget) {
        QModelIndex index = descriptorListWidget->selectionModel()->currentIndex();
        QUuid uid(index.data(Qt::UserRole).toString());
        ModelManager::instance().removeDescriptor(uid);
    }
}

void ModelManagerDialog::accept()
{
    // Remove some signals that will confuse things while closing
    ModelDescriptorListWidget *descriptorListWidget = ui->descriptorListParent->findChild<ModelDescriptorListWidget *>();
    if(descriptorListWidget) {
        disconnect(descriptorListWidget, SIGNAL(currentDescriptorChanged(QUuid)), this, SLOT(currentSelectionChanged(QUuid)));
    }

    // Let the current editor widget know that we're moving on (it should prompt the user to save)
    ModelDescriptorWidget *descriptorWidget = ui->descriptorParent->findChild<ModelDescriptorWidget *>();
    if(descriptorWidget) {
        descriptorWidget->close();
        ui->descriptorParent->layout()->removeWidget(descriptorWidget);
        descriptorWidget->deleteLater();
    }

    QDialog::accept();
}



} // namespace OpenSpeedShop
} // namespace Plugins
