#ifndef MODELDESCRIPTORWIDGET_H
#define MODELDESCRIPTORWIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include <QDomDocument>
#include <QUuid>

#include "ModelDescriptor.h"

namespace Plugins {
namespace OpenSpeedShop {

namespace Ui {
    class ModelDescriptorWidget;
}

class ModelDescriptorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModelDescriptorWidget(ModelDescriptor *modelDescriptor, QWidget *parent = 0);
    ~ModelDescriptorWidget();

    void close();

protected:
    void init();
    ModelDescriptor *m_ModelDescriptor;

protected slots:
    void on_cmbExperimentType_currentIndexChanged(int index);
    void on_buttonBox_clicked(QAbstractButton *button);

    void accept();
    void reject();

    bool hasChanged();

private:
    Ui::ModelDescriptorWidget *ui;
};


} // namespace OpenSpeedShop
} // namespace Plugins
#endif // MODELDESCRIPTORWIDGET_H
