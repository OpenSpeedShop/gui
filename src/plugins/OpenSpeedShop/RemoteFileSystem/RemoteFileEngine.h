#ifndef REMOTEFILEENGINE_H
#define REMOTEFILEENGINE_H

#include <QAbstractFileEngine>

namespace Plugins {
namespace OpenSpeedShop {

class RemoteFileEngine : public QAbstractFileEngine
{
public:
    RemoteFileEngine();

    /* if it's a file */
//    virtual void setFileName(const QString &file);
//    virtual bool rename(const QString &newName);
//    virtual bool remove();

    /* if it's a directory */
//    virtual bool mkdir(const QString &dirName, bool createParentDirectories) const;
//    virtual bool rmdir(const QString &dirName, bool recurseParentDirectories) const;
    virtual QStringList entryList(QDir::Filters filters, const QStringList &filterNames) const;

    /* all filesystem objects */
    virtual bool caseSensitive() const;
    virtual bool isRelativePath() const;
    virtual FileFlags fileFlags(FileFlags type) const;
    virtual uint ownerId(FileOwner owner) const;
    virtual QString owner(FileOwner owner) const;
    virtual QDateTime fileTime(FileTime time) const;


};

} // namespace OpenSpeedShop
} // namespace Plugins

#endif // REMOTEFILEENGINE_H
