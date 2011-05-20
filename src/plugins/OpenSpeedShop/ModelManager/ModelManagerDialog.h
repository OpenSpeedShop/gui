#ifndef MODELMANAGERDIALOG_H
#define MODELMANAGERDIALOG_H

#include <QDialog>

namespace Plugins {
namespace OpenSpeedShop {

class ModelDescriptor;
class ModelDescriptorWidget;
namespace Ui { class ModelManagerDialog; }

class ModelManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelManagerDialog(QWidget *parent = 0);
    ~ModelManagerDialog();

protected slots:
    void currentModelDescriptorChanged(ModelDescriptor *current);
    void on_btnCreate_clicked();
    void on_btnRemove_clicked();

protected:
    ModelDescriptorWidget *m_CurrentModelDescriptorWidget;

private:
    Ui::ModelManagerDialog *ui;
};

} // namespace OpenSpeedShop
} // namespace Plugins
#endif // MODELMANAGERDIALOG_H
