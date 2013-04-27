#ifndef OPENSPEEDSHOPWIDGET_H
#define OPENSPEEDSHOPWIDGET_H

#include <QtCore>
#include <QtGui>

#include "OpenSpeedShopLibrary.h"


namespace Core { namespace ActionManager { class Context; } }

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

signals:
    void active();

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

    Core::ActionManager::Context *m_Context;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // OPENSPEEDSHOPWIDGET_H
