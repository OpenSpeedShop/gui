#include "ModelDescriptorWidget.h"
#include "ui_ModelDescriptorWidget.h"

#include <QListWidgetItem>
#include <MainWindow/MainWindow.h>
#include "ConnectionManager/ConnectionManager.h"
#include "ConnectionManager/ServerAdapter.h"

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace OpenSpeedShop {

ModelDescriptorWidget::ModelDescriptorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelDescriptorWidget)
{
    ui->setupUi(this);
    m_ModelDescriptor = new ModelDescriptor(parent);
    init();
}

ModelDescriptorWidget::ModelDescriptorWidget(ModelDescriptor *modelDescriptor, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelDescriptorWidget)
{
    ui->setupUi(this);
    m_ModelDescriptor = modelDescriptor;
    init();

    ui->cmbExperimentType->setEnabled(false);
}

ModelDescriptorWidget::~ModelDescriptorWidget()
{
    delete ui;
}

void ModelDescriptorWidget::init()
{
    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();

    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify("Server not connected");
        return;
    }

    QStringList experimentTypes = serverAdapter->waitExperimentTypes();
    if(!experimentTypes.isEmpty()) {
        ui->cmbExperimentType->addItems(experimentTypes);
    }

    ui->txtModelName->setText(m_ModelDescriptor->name());
    ui->txtRowCount->setValue(m_ModelDescriptor->rowCount());

    int index = 0;
    if(!m_ModelDescriptor->experimentType().isEmpty()) {
        index = ui->cmbExperimentType->findText(m_ModelDescriptor->experimentType(), Qt::MatchExactly);
    }
    ui->cmbExperimentType->setCurrentIndex(index);
}

void ModelDescriptorWidget::on_cmbExperimentType_currentIndexChanged(int index)
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


    // Set the selected items if we're the same as the ModelDescriptor
    if(!m_ModelDescriptor->isEmpty() && !experimentType.compare(m_ModelDescriptor->experimentType(), Qt::CaseInsensitive)) {
        ui->lstModifiers->clearSelection();
        foreach(QString modifier, m_ModelDescriptor->modifiers()) {
            QList<QListWidgetItem *> items = ui->lstModifiers->findItems(modifier, Qt::MatchExactly);
            foreach(QListWidgetItem *item, items) {
                item->setSelected(true);
            }
        }

        ui->lstMetrics->clearSelection();
        foreach(QString metric, m_ModelDescriptor->metrics()) {
            QList<QListWidgetItem *> items = ui->lstMetrics->findItems(metric, Qt::MatchExactly);
            foreach(QListWidgetItem *item, items) {
                item->setSelected(true);
            }
        }
    }

}

void ModelDescriptorWidget::on_buttonBox_clicked(QAbstractButton *button)
{
    if(!button->text().compare("Reset", Qt::CaseInsensitive)) {
        init();
    }
}

void ModelDescriptorWidget::close()
{
    //TODO: Close the widget properly

    // Check to see if we've modified the modelDescriptor
    // If we have, ask the user if they would like to persist the changes.
}



} // namespace OpenSpeedShop
} // namespace Plugins
