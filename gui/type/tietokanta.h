#ifndef TIETOKANTA_H
#define TIETOKANTA_H

#include <QtSql>

#include <QtGui/QMessageBox>

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

    static bool checkVersion(const QString& version, const QString& table = QString());
    static QString getVersion(const QString& table = QString());
};

#endif // TIETOKANTA_H
