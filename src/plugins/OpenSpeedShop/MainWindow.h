#ifndef PLUGINS_OPENSPEEDSHOP_MAINWINDOW_H
#define PLUGINS_OPENSPEEDSHOP_MAINWINDOW_H

#include <WindowManager/IMainWindow.h>

namespace Plugins {
namespace OpenSpeedShop {

class OpenSpeedShopWidget;

class MainWindow : public Core::WindowManager::IMainWindow
{
    Q_OBJECT
    Q_INTERFACES(Core::WindowManager::IMainWindow)

public:
    explicit MainWindow(QObject *parent = 0);

    QWidget *mainWindowWidget();
    QString mainWindowName();
    int mainWindowPriority();
    QIcon mainWindowIcon();

    QWidget *createAboutWidget();

private:
    OpenSpeedShopWidget *m_Widget;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // PLUGINS_OPENSPEEDSHOP_MAINWINDOW_H
