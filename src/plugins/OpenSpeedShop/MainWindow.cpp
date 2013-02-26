#include "MainWindow.h"

#include "OpenSpeedShopWidget.h"
#include "AboutWidget.h"

namespace Plugins {
namespace OpenSpeedShop {

MainWindow::MainWindow(QObject *parent) :
    Core::WindowManager::IMainWindow(parent),
    m_Widget(NULL)
{
}

QWidget *MainWindow::mainWindowWidget()
{
    if(!m_Widget) {
        m_Widget = new OpenSpeedShopWidget();
    }
    return m_Widget;
}

QString MainWindow::mainWindowName()
{
    return tr("O|SS");
}

int MainWindow::mainWindowPriority()
{
    return 32;
}

QIcon MainWindow::mainWindowIcon()
{
    return mainWindowWidget()->windowIcon();
}

QWidget *MainWindow::createAboutWidget()
{
    return new AboutWidget();
}

} // namespace OpenSpeedShop
} // namespace Plugins
