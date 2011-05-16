#include "RemoteFileEngineHandler.h"
#include "RemoteFileEngine.h"

namespace Plugins {
namespace OpenSpeedShop {

QAbstractFileEngine *RemoteFileEngineHandler::create(const QString &fileName) const
{
    if(fileName.startsWith("openss://", Qt::CaseInsensitive)) {
        return new RemoteFileEngine(fileName);
    }

    return NULL;
}

} // namespace OpenSpeedShop
} // namespace Plugins
