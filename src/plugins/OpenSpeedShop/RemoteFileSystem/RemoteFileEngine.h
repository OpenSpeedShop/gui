#ifndef REMOTEFILEENGINE_H
#define REMOTEFILEENGINE_H

#include <QAbstractFileEngine>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QDir>

#include "RemoteFile.h"
#include "RemoteFileCache.h"

namespace Plugins {
namespace OpenSpeedShop {

class RemoteFileEngine : public QAbstractFileEngine
{
public:
    RemoteFileEngine();
    RemoteFileEngine(const QString &fileName);

    /* if it's a file */
    QString fileName(FileName file) const;
    void setFileName(const QString &file);
    bool rename(const QString &newName);
    bool remove();

    /* if it's a directory */
    bool mkdir(const QString &dirName, bool createParentDirectories) const;
    bool rmdir(const QString &dirName, bool recurseParentDirectories) const;
    QStringList entryList(QDir::Filters filters, const QStringList &filterNames);

    /* all filesystem objects */
    bool caseSensitive() const;
    bool isRelativePath() const;
    bool isSequential() const;
    QAbstractFileEngine::FileFlags fileFlags(QAbstractFileEngine::FileFlags type);
    uint ownerId(QAbstractFileEngine::FileOwner owner);
    QString owner(QAbstractFileEngine::FileOwner owner);
    QDateTime fileTime(QAbstractFileEngine::FileTime time);


private:
    QString m_FileName;
    RemoteFileCache m_RemoteFileCache;

};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // REMOTEFILEENGINE_H
