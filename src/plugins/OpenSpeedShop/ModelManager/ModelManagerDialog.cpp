#include "ModelManagerDialog.h"
#include "ui_ModelManagerDialog.h"

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

    QWidget *listParent = ui->lstModelDescriptors->parentWidget();
    delete(ui->lstModelDescriptors);
    ModelDescriptorListWidget *descriptorListWidget = ModelManager::instance()->createDescriptorListWidget(listParent);
    listParent->layout()->addWidget(descriptorListWidget);
    ui->lstModelDescriptors = descriptorListWidget;
    connect(descriptorListWidget, SIGNAL(currentModelDescriptorChanged(const QUuid&)),
            this, SLOT(currentModelDescriptorChanged(const QUuid&)));

}

ModelManagerDialog::~ModelManagerDialog()
{
    delete ui;
}

void ModelManagerDialog::currentModelDescriptorChanged(const QUuid &current)
{
    ModelDescriptorWidget *modelDescriptorWidget = qobject_cast<ModelDescriptorWidget *>(ui->modelDescriptor);
    if(modelDescriptorWidget) {
        // Let the current editor widget know that we're moving on (it should prompt the user to save)
        modelDescriptorWidget->close();
    }

    /* This is separate from the above casted operations because it might not always be a ModelDescriptorWidget --the place-
       holder widget is just a QWidget. */
    if(ui->modelDescriptor) {
        delete ui->modelDescriptor;
    }

    ui->modelDescriptor = ModelManager::instance()->createDescriptorWidget(current, this);
}

void ModelManagerDialog::on_btnCreate_clicked()
{
    QUuid descriptorId = ModelManager::instance()->createDescriptor();

    //TODO: Select the new item in the view, which sets the editor to display it
}

void ModelManagerDialog::on_btnRemove_clicked()
{
}


} // namespace OpenSpeedShop
} // namespace Plugins
