#include "ExperimentWidget.h"
#include "ui_ExperimentWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

ExperimentWidget::ExperimentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentWidget)
{
    ui->setupUi(this);
}

ExperimentWidget::~ExperimentWidget()
{
    delete ui;
}

} // namespace OpenSpeedShop
} // namespace Plugins
