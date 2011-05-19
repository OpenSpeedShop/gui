#include "ModelManagerDialog.h"
#include "ui_ModelManagerDialog.h"

#include <QSplitter>
#include "ModelManager.h"
#include "ModelDescriptorWidget.h"
#include "ModelListWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

ModelManagerDialog::ModelManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelManagerDialog)
{
    ui->setupUi(this);

    delete(ui->lstModelDescriptors);
    ModelListWidget *modelListWidget = ModelManager::instance()->createModelListWidget(QString(), this);
    ui->lstModelDescriptors = modelListWidget;
    connect(modelListWidget, SIGNAL(currentModelDescriptorChanged(ModelDescriptor*)),
            this, SLOT(currentModelDescriptorChanged(ModelDescriptor*)));

}

ModelManagerDialog::~ModelManagerDialog()
{
    delete ui;
}

void ModelManagerDialog::currentModelDescriptorChanged(ModelDescriptor *current)
{
    ModelDescriptorWidget *modelDescriptorWidget = qobject_cast<ModelDescriptorWidget *>(ui->modelDescriptor);
    if(modelDescriptorWidget) {
        // Let the current editor widget know that we're moving on (it should prompt the user to save)
        modelDescriptorWidget->close();
    }

    if(ui->modelDescriptor) {
        delete ui->modelDescriptor;
    }

    ui->modelDescriptor = new ModelDescriptorWidget(current, this);
}


} // namespace OpenSpeedShop
} // namespace Plugins
