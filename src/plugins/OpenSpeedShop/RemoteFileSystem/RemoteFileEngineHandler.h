#ifndef REMOTEFILEENGINEHANDLER_H
#define REMOTEFILEENGINEHANDLER_H

#include <QAbstractFileEngineHandler>

namespace Plugins {
namespace OpenSpeedShop {

class RemoteFileEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString &fileName) const;
};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // REMOTEFILEENGINEHANDLER_H
