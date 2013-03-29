#ifndef MAKROT_H
#define MAKROT_H

#include <QtCore/QString>

#define VERSION "1.0a"

#define INFO(W, M) QMessageBox::information(W, _("Tulospalvelu"), M)
#define _(S) QString::fromUtf8(S)

#ifdef DEBUG
#define SQL_EXEC(Q, R) { QTime __ti; __ti.start(); if (!Q.exec()) { qWarning() << Q.lastQuery() ; qWarning() << Q.lastError() ; QSqlDatabase::database().rollback(); return R; } qDebug() << __ti.elapsed() << Q.lastQuery(); }
#else
#define SQL_EXEC(Q, R) if (!Q.exec()) { qWarning() << Q.lastQuery() ; qWarning() << Q.lastError() ; QSqlDatabase::database().rollback(); return R; }
#endif

#endif // MAKROT_H
