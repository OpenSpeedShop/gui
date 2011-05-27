#include "ModelDescriptorWidget.h"
#include "ui_ModelDescriptorWidget.h"

#include <QMessageBox>
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

    ui->cmbExperimentType->setEnabled(m_ModelDescriptor->isEmpty());
}

ModelDescriptorWidget::~ModelDescriptorWidget()
{
    delete ui;
}

void ModelDescriptorWidget::init()
{
    ui->txtModelName->setText(m_ModelDescriptor->name());
    ui->txtRowCount->setValue(m_ModelDescriptor->rowCount());

    // We need a server connection to populate the combo box
    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();
    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify("Server not connected");
        return;
    }

    // Populate the combo box with possible values
    QStringList experimentTypes = serverAdapter->waitExperimentTypes();
    if(!experimentTypes.isEmpty()) {
        experimentTypes.sort();
        ui->cmbExperimentType->addItems(experimentTypes);
    }

    // Set the combo box selection to match the descriptor
    int index = 0;
    if(!m_ModelDescriptor->experimentType().isEmpty()) {
        index = ui->cmbExperimentType->findText(m_ModelDescriptor->experimentType(), Qt::MatchExactly);
    }
    ui->cmbExperimentType->setCurrentIndex(index);
}

void ModelDescriptorWidget::on_cmbExperimentType_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    // We need a valid server connection in order
    ServerAdapter *serverAdapter = ConnectionManager::instance()->currentServerAdapter();
    if(!serverAdapter) {
        Core::MainWindow::MainWindow::instance()->notify(tr("Server not connected"));
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
    // Check to see if we've modified the modelDescriptor
    if(hasChanged()) {
        // If we have, ask the user if they would like to persist the changes.
        QMessageBox msg(QMessageBox::Question,
                        tr("Model has changed"),
                        tr("Would you like to persist the settings?"),
                        QMessageBox::Yes|QMessageBox::No);
        if(msg.exec() == QMessageBox::Yes) {
            accept();
        } else {
            reject();
        }
    }
}

bool ModelDescriptorWidget::hasChanged()
{
    if(m_ModelDescriptor->name().compare(ui->txtModelName->text()))
        return true;
    if(m_ModelDescriptor->experimentType().compare(ui->cmbExperimentType->currentText()))
        return true;
    if((int)m_ModelDescriptor->rowCount() != ui->txtRowCount->value())
        return true;

    // Check the modifiers list lengths to shortcut the check
    QList<QListWidgetItem *> selectedModifiers = ui->lstModifiers->selectedItems();
    if(selectedModifiers.count() != m_ModelDescriptor->modifiers().count())
        return true;

    // Check the metrics list lengths to shortcut the check
    QList<QListWidgetItem *> selectedMetrics = ui->lstMetrics->selectedItems();
    if(selectedMetrics.count() != m_ModelDescriptor->metrics().count())
        return true;

    // Do a deeper comparison of the two lists
    QStringList modifiers;
    foreach(QListWidgetItem *item, selectedModifiers) {
        QString itemText = item->text();
        modifiers.append(itemText);
        if(!m_ModelDescriptor->modifiers().contains(itemText)) {
            return true;
        }
    }
    foreach(QString string, m_ModelDescriptor->modifiers()) {
        if(!modifiers.contains(string)) {
            return true;
        }
    }

    // Do a deeper comparison of the two lists
    QStringList metrics;
    foreach(QListWidgetItem *item, selectedMetrics) {
        QString itemText = item->text();
        metrics.append(itemText);
        if(!m_ModelDescriptor->metrics().contains(itemText)) {
            return true;
        }
    }
    foreach(QString string, m_ModelDescriptor->metrics()) {
        if(!metrics.contains(string)) {
            return true;
        }
    }

    return false;
}

void ModelDescriptorWidget::accept()
{
    m_ModelDescriptor->setName(ui->txtModelName->text());
    m_ModelDescriptor->setExperimentType(ui->cmbExperimentType->currentText());
    m_ModelDescriptor->setRowCount((quint64)ui->txtRowCount->value());

    m_ModelDescriptor->clearModifiers();
    foreach(QListWidgetItem *item, ui->lstModifiers->selectedItems()) {
        m_ModelDescriptor->insertModifier(item->text());
    }

    m_ModelDescriptor->clearMetrics();
    foreach(QListWidgetItem *item, ui->lstMetrics->selectedItems()) {
        m_ModelDescriptor->insertMetric(item->text());
    }
}

void ModelDescriptorWidget::reject()
{

}

} // namespace OpenSpeedShop
} // namespace Plugins
