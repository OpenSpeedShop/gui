#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QSplitter>
#include <QtHelp>

namespace Plugins {
namespace Help {

class HelpWidget : public QSplitter
{
    Q_OBJECT
public:
    explicit HelpWidget(QHelpEngine *helpEngine, QWidget *parent = 0);

    bool isBackwardAvailable();
    bool isForwardAvailable();

signals:
    void backwardAvailable(bool);
    void forwardAvailable(bool);

public slots:
    void backward();
    void forward();

protected:
    void initSideBar();
    void initDocumentTabs();

protected slots:
    void linkActivated(QUrl url, QString string = QString());
    void search();
    void tabCloseRequested(int);
    void tabCurrentChanged(int);

    void browserBackwardAvailable(bool);
    void browserForwardAvailable(bool);

private:
    QHelpEngine *m_HelpEngine;
    QTabWidget m_Documents;

};

} // namespace Help
} // namespace Plugins

#endif // HELPWIDGET_H
