#ifndef TIETOKANTA_H
#define TIETOKANTA_H

#include <QtSql>

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
};

#endif // TIETOKANTA_H
