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

    static void vieTulokset(const Tapahtuma* tapahtuma, const QString& fileName);
    static bool tuoTulokset(const Tapahtuma* tapahtuma, const QString& fileName);

    static bool checkVersion(const QString& version);
    static QString getVersion();
};

#endif // TIETOKANTA_H
