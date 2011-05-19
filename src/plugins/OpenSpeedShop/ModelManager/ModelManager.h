#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <QObject>
#include <QDomDocument>


namespace Plugins {
namespace OpenSpeedShop {

class ModelDescriptor;
class ModelDescriptorWidget;
class ModelListWidget;

class ModelManager : public QObject
{
    Q_OBJECT
public:
    static ModelManager *instance();

    void fromXml(QString filePath);
    void fromXml(QDomDocument document);

    ModelListWidget *createModelListWidget(QString filter = QString(), QWidget *parent = NULL);

protected:
    explicit ModelManager(QObject *parent = 0);

    QList<ModelDescriptor *> m_ModelDescriptors;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // MODELMANAGER_H
