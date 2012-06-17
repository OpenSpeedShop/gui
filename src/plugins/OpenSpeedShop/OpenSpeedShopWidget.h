#ifndef OPENSPEEDSHOPWIDGET_H
#define OPENSPEEDSHOPWIDGET_H

#include <QtCore>
#include <QtGui>

#include "OpenSpeedShopLibrary.h"

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

    QAction *m_CreateExperiment;
    QAction *m_LoadExperiment;
    QAction *m_CloseExperiment;

protected slots:
    void tabTitleChanged();

    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    Ui::OpenSpeedShopWidget *ui;
    QString m_StyleSheet;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // OPENSPEEDSHOPWIDGET_H
