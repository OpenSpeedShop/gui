#ifndef OPENSPEEDSHOPWIDGET_H
#define OPENSPEEDSHOPWIDGET_H

#include <QTabWidget>
#include <MainWindow/MainWindow.h>
#include "Experiment/ExperimentWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui {
    class OpenSpeedShopWidget;
}

class OpenSpeedShopWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit OpenSpeedShopWidget(QWidget *parent = 0);
    ~OpenSpeedShopWidget();

public slots:
    void createExperiment();
    void loadExperiment();

private:
    Ui::OpenSpeedShopWidget *ui;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // OPENSPEEDSHOPWIDGET_H
