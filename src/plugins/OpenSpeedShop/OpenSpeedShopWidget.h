#ifndef OPENSPEEDSHOPWIDGET_H
#define OPENSPEEDSHOPWIDGET_H

#include <QTabWidget>
#include <QAction>
#include <MainWindow/MainWindow.h>
#include "OpenSpeedShopLibrary.h"
#include "Experiment/ExperimentWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui {
    class OpenSpeedShopWidget;
}

class OPENSPEEDSHOP_EXPORT OpenSpeedShopWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit OpenSpeedShopWidget(QWidget *parent = 0);
    ~OpenSpeedShopWidget();

public slots:
    void createExperiment();
    void loadExperiment();
    void closeExperiment(int index = -1);

protected:
    void tabInserted(int index);
    void tabRemoved(int index);

    QAction *m_CloseExperiment;

protected slots:
    void tabTitleChanged();

private:
    Ui::OpenSpeedShopWidget *ui;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // OPENSPEEDSHOPWIDGET_H
