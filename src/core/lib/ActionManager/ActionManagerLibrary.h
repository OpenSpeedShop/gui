#ifndef ACTIONMANAGERLIBRARY_H
#define ACTIONMANAGERLIBRARY_H

#include <QtCore/QtGlobal>

#if defined(ACTIONMANAGER_LIBRARY)
#  define ACTIONMANAGER_EXPORT Q_DECL_EXPORT
#else
#  define ACTIONMANAGER_EXPORT Q_DECL_IMPORT
#endif

#endif // ACTIONMANAGERLIBRARY_H