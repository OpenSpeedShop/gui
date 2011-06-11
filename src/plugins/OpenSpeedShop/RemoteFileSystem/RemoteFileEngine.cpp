#include "RemoteFileEngine.h"

#include <QRegExp>

#ifdef QT_DEBUG
#  include <QDebug>
#endif

namespace Plugins {
namespace OpenSpeedShop {

RemoteFileEngine::RemoteFileEngine()
{
}

RemoteFileEngine::RemoteFileEngine(const QString &fileName)
{
    setFileName(fileName);
}

/*! \reimp */
QString RemoteFileEngine::fileName(QAbstractFileEngine::FileName file) const
{
    static QRegExp fileNameParts("^(\\w+):://(.*)[/\\\\]([a-zA-Z0-9\\_\\-\\s]+)[/\\\\]*\\s*$");

    switch(file) {
    case QAbstractFileEngine::DefaultName:
        return m_FileName;
    case QAbstractFileEngine::BaseName:
        if(fileNameParts.exactMatch(m_FileName))
            return fileNameParts.cap(2);
    case QAbstractFileEngine::PathName:
        if(fileNameParts.exactMatch(m_FileName))
            return fileNameParts.cap(3);
    case QAbstractFileEngine::AbsoluteName:
        if(fileNameParts.exactMatch(m_FileName))
            return QString("%1/%2").arg(fileNameParts.cap(3), fileNameParts.cap(2));
    case QAbstractFileEngine::AbsolutePathName:
        if(fileNameParts.exactMatch(m_FileName))
            return fileNameParts.cap(3);
    default:
        return QAbstractFileEngine::fileName(file);
    }
}


/*! \reimp */
void RemoteFileEngine::setFileName(const QString &file)
{
    m_FileName = file;
}

/*! \reimp */
bool RemoteFileEngine::rename(const QString &newName)
{
    Q_UNUSED(newName)
    return false;
}

/*! \reimp */
bool RemoteFileEngine::remove()
{
    return false;
}

/*! \reimp */
bool RemoteFileEngine::mkdir(const QString &dirName, bool createParentDirectories) const
{
    Q_UNUSED(dirName)
    Q_UNUSED(createParentDirectories)
    return false;
}

/*! \reimp */
bool RemoteFileEngine::rmdir(const QString &dirName, bool recurseParentDirectories) const
{
    Q_UNUSED(dirName)
    Q_UNUSED(recurseParentDirectories)
    return false;
}

/*! \reimp */
QStringList RemoteFileEngine::entryList(QDir::Filters filters, const QStringList &filterNames)
{
    Q_UNUSED(filters)
    Q_UNUSED(filterNames)

    QString fileName = this->fileName(QAbstractFileEngine::BaseName);
    QString pathName = this->fileName(QAbstractFileEngine::PathName);
    RemoteFile remoteFile = m_RemoteFileCache.file(fileName, pathName);

    if(remoteFile.type() != "directory") {
        return QStringList();
    }

    //TODO: deal with the supplied filters

    QMap<QString, RemoteFile> listing = m_RemoteFileCache.listing(remoteFile.path());
    return QStringList(listing.keys());
}


/*! \reimp */
bool RemoteFileEngine::caseSensitive() const
{
    return true;
}

/*! \reimp */
bool RemoteFileEngine::isRelativePath() const
{
    return false;
}

/*! \reimp */
bool RemoteFileEngine::isSequential() const
{
    return false;
}

/*! \reimp */
QAbstractFileEngine::FileFlags RemoteFileEngine::fileFlags(QAbstractFileEngine::FileFlags type)
{
    if(type & QAbstractFileEngine::Refresh) {
        //TODO: Force a refresh of this file from the server
    }

    QString fileName = this->fileName(QAbstractFileEngine::BaseName);
    QString pathName = this->fileName(QAbstractFileEngine::PathName);
    RemoteFile remoteFile = m_RemoteFileCache.file(fileName, pathName);

    QAbstractFileEngine::FileFlags retval = 0;
    if(fileName.startsWith(QLatin1Char('.')))      retval |= QAbstractFileEngine::HiddenFlag;
    if(remoteFile.isValid())                       retval |= QAbstractFileEngine::ExistsFlag;
    if(m_FileName == QLatin1String("openss::///")) retval |= QAbstractFileEngine::RootFlag;

    if(remoteFile.type() == "file")                retval |= QAbstractFileEngine::FileType;
    else if(remoteFile.type() == "directory")      retval |= QAbstractFileEngine::DirectoryType;
    else if(remoteFile.type() == "link")           retval |= QAbstractFileEngine::LinkType;
//    else                                           retval |= QAbstractFileEngine::

    quint16 filePermissions = remoteFile.permissions();
    if(filePermissions & 0x400)                    retval |= QAbstractFileEngine::ReadOwnerPerm;
    if(filePermissions & 0x200)                    retval |= QAbstractFileEngine::WriteOwnerPerm;
    if(filePermissions & 0x100)                    retval |= QAbstractFileEngine::ExeOwnerPerm;
    if(filePermissions & 0x040)                    retval |= QAbstractFileEngine::ReadGroupPerm;
    if(filePermissions & 0x020)                    retval |= QAbstractFileEngine::WriteGroupPerm;
    if(filePermissions & 0x010)                    retval |= QAbstractFileEngine::ExeGroupPerm;
    if(filePermissions & 0x004)                    retval |= QAbstractFileEngine::ReadOtherPerm;
    if(filePermissions & 0x002)                    retval |= QAbstractFileEngine::WriteOtherPerm;
    if(filePermissions & 0x001)                    retval |= QAbstractFileEngine::ExeOtherPerm;

#ifdef QT_DEBUG
    if(type & QAbstractFileEngine::ReadUserPerm)
        qDebug() << __FILE__ << __LINE__ << "Request for ReadUserPerm on file" << m_FileName;
    if(type & QAbstractFileEngine::WriteUserPerm)
        qDebug() << __FILE__ << __LINE__ << "Request for WriteUserPerm on file" << m_FileName;
    if(type & QAbstractFileEngine::ExeUserPerm)
        qDebug() << __FILE__ << __LINE__ << "Request for ExeUserPerm on file" << m_FileName;
#endif

    return retval;
}

/*! \reimp */
uint RemoteFileEngine::ownerId(QAbstractFileEngine::FileOwner owner)
{
    QString fileName = this->fileName(QAbstractFileEngine::BaseName);
    QString pathName = this->fileName(QAbstractFileEngine::PathName);
    RemoteFile remoteFile = m_RemoteFileCache.file(fileName, pathName);

    if(owner == QAbstractFileEngine::OwnerUser) {
        return (uint)remoteFile.userId();
    } else if(owner == QAbstractFileEngine::OwnerGroup) {
        return (uint)remoteFile.groupId();
    }

    return -1;
}

/*! \reimp */
QString RemoteFileEngine::owner(QAbstractFileEngine::FileOwner owner)
{
#ifdef QT_DEBUG
    if(owner == QAbstractFileEngine::OwnerUser) {
        qDebug() << __FILE__ << __LINE__ << "Request for owner name on file" << m_FileName;
    } else if(owner == QAbstractFileEngine::OwnerGroup) {
        qDebug() << __FILE__ << __LINE__ << "Request for group name on file" << m_FileName;
    }

#endif
    return QString();
}

/*! \reimp */
QDateTime RemoteFileEngine::fileTime(QAbstractFileEngine::FileTime time)
{
    QString fileName = this->fileName(QAbstractFileEngine::BaseName);
    QString pathName = this->fileName(QAbstractFileEngine::PathName);
    RemoteFile remoteFile = m_RemoteFileCache.file(fileName, pathName);

    if(time == QAbstractFileEngine::CreationTime) {
        return remoteFile.createdTime();
    } else if(time == QAbstractFileEngine::ModificationTime) {
        return remoteFile.modifiedTime();
    } else if(time == QAbstractFileEngine::AccessTime) {
        return remoteFile.accessTime();
    }

    return QDateTime();
}


} // namespace OpenSpeedShop
} // namespace Plugins
