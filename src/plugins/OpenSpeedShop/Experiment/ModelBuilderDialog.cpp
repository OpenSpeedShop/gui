#include "ModelBuilderDialog.h"
#include "ui_ModelBuilderDialog.h"

#include <QListWidgetItem>
#include <MainWindow/MainWindow.h>
#include "ConnectionManager/ConnectionManager.h"
#include "ConnectionManager/ServerAdapter.h"

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace OpenSpeedShop {

ModelBuilderDialog::ModelBuilderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelBuilderDialog)
{
    ui->setupUi(this);

    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();

    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify("Server not connected");
        return;
    }

    QStringList experimentTypes = serverAdapter->waitExperimentTypes();
    if(!experimentTypes.isEmpty()) {
        ui->cmbExperimentType->addItems(experimentTypes);
    }

    ui->cmbExperimentType->setCurrentIndex(0);
}

ModelBuilderDialog::~ModelBuilderDialog()
{
    delete ui;
}

void ModelBuilderDialog::on_cmbExperimentType_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();

    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify("Server not connected");
        return;
    }

    QString experimentType = ui->cmbExperimentType->currentText();

    ui->lstModifiers->clear();
    QMap<QString,QString> experimentTypeModifiers = serverAdapter->waitExperimentTypeModifiers(experimentType);
    foreach(QString key, experimentTypeModifiers.keys()) {
        QListWidgetItem *item = new QListWidgetItem(key);
        item->setToolTip(QString("'%1' %2").arg(key).arg(experimentTypeModifiers[key]));
        ui->lstModifiers->addItem(item);
    }
    ui->lstModifiers->sortItems(Qt::AscendingOrder);

    ui->lstMetrics->clear();
    QMap<QString,QString> experimentTypeMetrics = serverAdapter->waitExperimentTypeMetrics(experimentType);
    foreach(QString key, experimentTypeMetrics.keys()) {
        QListWidgetItem *item = new QListWidgetItem(key);
        item->setToolTip(QString("'%1' %2").arg(key).arg(experimentTypeModifiers[key]));
        ui->lstMetrics->addItem(item);
    }
    ui->lstMetrics->sortItems(Qt::AscendingOrder);
}


} // namespace OpenSpeedShop
} // namespace Plugins
