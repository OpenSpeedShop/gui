#ifndef EXPERIMENTWIDGET_H
#define EXPERIMENTWIDGET_H

#include <QWidget>

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui { class ExperimentWidget; }

class ExperimentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExperimentWidget(QWidget *parent = 0);
    ~ExperimentWidget();

    void create();
    void load();

protected slots:
    void on_btnAddModel_clicked();

private:
    Ui::ExperimentWidget *ui;

};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // EXPERIMENTWIDGET_H
