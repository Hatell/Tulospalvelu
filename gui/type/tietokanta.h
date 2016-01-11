#ifndef TIETOKANTA_H
#define TIETOKANTA_H

#include <QtSql>

#include <QtWidgets/QMessageBox>

#include "type/tapahtuma.h"

#include "makrot.h"

class Tietokanta
{
public:

    static void buildSQLite();
    static void buildMySQL();

    static void insertData();

    static void dropTables();

    static void luoKilpailijatietokanta(const QString& fn);
    static bool tuoKilpailijatietokanta(const QString& fn);

    static void SQLiteLuoKilpailijatietokanta();
    static bool SQLiteTuoKilpailijatietokanta();

    static void vieTulokset(const Tapahtuma* tapahtuma, const QString& fileName);
    static bool tuoTulokset(const Tapahtuma* tapahtuma, const QString& fileName);

    static void SQLiteVieTulokset(const Tapahtuma* tapahtuma);
    static bool SQLiteTuoTulokset(const Tapahtuma* tapahtuma);

    static bool checkVersion(const QString& version, const QString& table = QString());
    static QString getVersion(const QString& table = QString());

    static void backup(const QString& toTarget  = _("tulospalvelu_bak_%1.sqlite3").arg(QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss")));
};

#endif // TIETOKANTA_H
