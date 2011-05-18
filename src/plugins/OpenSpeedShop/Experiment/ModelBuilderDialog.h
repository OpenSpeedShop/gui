#ifndef MODELBUILDERDIALOG_H
#define MODELBUILDERDIALOG_H

#include <QDialog>

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui {
    class ModelBuilderDialog;
}

class ModelBuilderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelBuilderDialog(QWidget *parent = 0);
    ~ModelBuilderDialog();

protected slots:
    void on_cmbExperimentType_currentIndexChanged(int index);

private:
    Ui::ModelBuilderDialog *ui;
};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // MODELBUILDERDIALOG_H
