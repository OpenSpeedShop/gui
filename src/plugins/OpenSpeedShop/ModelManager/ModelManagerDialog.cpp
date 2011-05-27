#include "ModelManagerDialog.h"
#include "ui_ModelManagerDialog.h"

#include <SettingManager/SettingManager.h>

#include "ModelManager.h"
#include "ModelDescriptorWidget.h"
#include "ModelDescriptorListWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

ModelManagerDialog::ModelManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelManagerDialog)
{
    ui->setupUi(this);

    // Create and add the descriptor list widget to the dialog
    ModelDescriptorListWidget *descriptorListWidget =
            ModelManager::instance()->createDescriptorListWidget(ui->descriptorListParent);
    ui->descriptorListParent->layout()->addWidget(descriptorListWidget);
    ui->descriptorListParent->layout()->setMargin(0);
    connect(descriptorListWidget, SIGNAL(currentSelectionChanged(QUuid)),
            this, SLOT(currentSelectionChanged(QUuid)));

    Core::SettingManager::SettingManager *settingManager = Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins");
    settingManager->beginGroup("OpenSpeedShop");
    settingManager->beginGroup("ModelManagerDialog");

    restoreGeometry(settingManager->value("windowGeometry", saveGeometry()).toByteArray());
    ui->splitter->restoreGeometry(settingManager->value("splitterGeometry", ui->splitter->saveGeometry()).toByteArray());
    ui->splitter->restoreState(settingManager->value("splitterState", ui->splitter->saveState()).toByteArray());

    settingManager->endGroup();
    settingManager->endGroup();
    settingManager->endGroup();
}

ModelManagerDialog::~ModelManagerDialog()
{
    Core::SettingManager::SettingManager *settingManager = Core::SettingManager::SettingManager::instance();

    settingManager->beginGroup("Plugins");
    settingManager->beginGroup("OpenSpeedShop");
    settingManager->beginGroup("ModelManagerDialog");

    settingManager->setValue("windowGeometry", saveGeometry());
    settingManager->setValue("splitterGeometry", ui->splitter->saveGeometry());
    settingManager->setValue("splitterState", ui->splitter->saveState());

    settingManager->endGroup();
    settingManager->endGroup();
    settingManager->endGroup();

    delete ui;
}

void ModelManagerDialog::currentSelectionChanged(const QUuid &current)
{
    // Let the current editor widget know that we're moving on (it should prompt the user to save)
    ModelDescriptorWidget *descriptorWidget = ui->descriptorParent->findChild<ModelDescriptorWidget *>();
    if(descriptorWidget) {
        descriptorWidget->close();
        ui->descriptorParent->layout()->removeWidget(descriptorWidget);
        descriptorWidget->deleteLater();
    }

    if(!current.isNull()) {
        QWidget *descriptor = ModelManager::instance()->createDescriptorWidget(current, this);
        ui->descriptorParent->layout()->addWidget(descriptor);
        ui->descriptorParent->layout()->setMargin(0);
    }
}

void ModelManagerDialog::on_btnCreate_clicked()
{
    QUuid descriptorId = ModelManager::instance()->createDescriptor();

    //TODO: Select the new item in the view, which sets the editor to display it
    ModelDescriptorListWidget *descriptorListWidget = ui->descriptorListParent->findChild<ModelDescriptorListWidget *>();
    if(descriptorListWidget) {
        descriptorListWidget->selectRow(descriptorId);
    }
}

void ModelManagerDialog::on_btnRemove_clicked()
{
}


} // namespace OpenSpeedShop
} // namespace Plugins
