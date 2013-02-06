#include "tietokanta.h"


void Tietokanta::buildSQLite()
{
    QSqlQuery query;
    QStringList tables;

    dropTables();

tables
<<
        "CREATE TABLE tapahtuma ("
        "   id INTEGER NOT NULL,"
        "   nimi TEXT NOT NULL,"
        "   PRIMARY KEY (id)"
        ")"
<<
        "CREATE TABLE kilpailija ("
        "   id INTEGER NOT NULL,"
        "   nimi TEXT NOT NULL,"
        "   PRIMARY KEY (id)"
        ")"
<<
       "CREATE TABLE emit ("
       "    id VARCHAR(8) NOT NULL,"
       "    vuosi INTEGER(2) NOT NULL,"
       "    kuukausi INTEGER(2) NOT NULL,"
       "    kilpailija INTEGER,"
       "    FOREIGN KEY (kilpailija) REFERENCES kilpailija(id)"
       "        ON UPDATE CASCADE"
       "        ON DELETE SET NULL,"
       "    PRIMARY KEY (id)"
       ")"
 <<
        "CREATE TABLE sarja ("
        "   id INTEGER NOT NULL,"
        "   nimi TEXT NOT NULL,"
        "   tapahtuma INTEGER NOT NULL,"
        "   FOREIGN KEY (tapahtuma) REFERENCES tapahtuma(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   PRIMARY KEY (id)"
        ")"
<<
        "CREATE TABLE tulos_tila ("
        "   id INTEGER NOT NULL,"
        "   nimi TEXT NOT NULL,"
        "   PRIMARY KEY (id)"
        ")"
<<
        "CREATE TABLE tulos ("
        "    id INTEGER NOT NULL,"
        "   tapahtuma INTEGER NOT NULL,"
        "   emit VARCHAR(8) NOT NULL,"
        "   kilpailija INTEGER NOT NULL,"
        "   sarja INTEGER NOT NULL,"
        "   tila INTEGER NOT NULL,"
        "   aika TIME NOT NULL,"
        "   FOREIGN KEY (tapahtuma) REFERENCES tapahtuma(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   FOREIGN KEY (emit) REFERENCES emit(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   FOREIGN KEY (kilpailija) REFERENCES kilpailija(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   FOREIGN KEY (sarja) REFERENCES sarja(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   FOREIGN KEY (tila) REFERENCES tulos_tila(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE RESTRICT,"
        "   PRIMARY KEY (id)"
        ")"
<<
        "CREATE TABLE valiaika ("
        "   id INTEGER NOT NULL,"
        "   tulos INTEGER NOT NULL,"
        "   numero INTEGER NOT NULL,"
        "   koodi INTEGER NOT NULL,"
        "   aika TIME NOT NULL,"
        "   FOREIGN KEY (tulos) REFERENCES tulos(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   PRIMARY KEY (id)"
        ")"
<<
        "CREATE UNIQUE INDEX valiaika_index ON valiaika (tulos, numero, koodi, aika)"
<<
        "CREATE TABLE rasti ("
        "   id INTEGER NOT NULL,"
        "   sarja INTEGER NOT NULL,"
        "   numero INTEGER NOT NULL,"
        "   koodi INTEGER NOT NULL,"
        "   FOREIGN KEY (sarja) REFERENCES sarja(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   PRIMARY KEY (id)"
        ")"
<<
        "CREATE TABLE luettu_emit ("
        "   id INTEGER NOT NULL,"
        "   tapahtuma INTEGER NOT NULL,"
        "   emit VARCHAR(8) NOT NULL,"
        "   luettu DATETIME NOT NULL,"
        "   tulos INTEGER,"
        "   FOREIGN KEY (tapahtuma) REFERENCES tapahtuma(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   FOREIGN KEY (emit) REFERENCES emit(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   FOREIGN KEY (tulos) REFERENCES tulos(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   PRIMARY KEY (id)"
        ")"
<<
        "CREATE TABLE luettu_emit_rasti ("
        "   id INTEGER NOT NULL,"
        "   luettu_emit INTEGER NOT NULL,"
        "   numero INTEGER NOT NULL,"
        "   koodi INTEGER NOT NULL,"
        "   aika INTEGER NOT NULL,"
        "   FOREIGN KEY (luettu_emit) REFERENCES luettu_emit(id)"
        "       ON UPDATE CASCADE"
        "       ON DELETE CASCADE,"
        "   PRIMARY KEY (id)"
        ")"
;

    foreach (QString table, tables) {
        query.prepare(table);

        SQL_EXEC(query,);
    }

    insertData();
}

void Tietokanta::buildMySQL()
{
    // FIXME tee loppuun

    dropTables();


    insertData();
}

void Tietokanta::dropTables()
{
    return;
    /* FIXME tarvitaanko tätä
    QSqlQuery query;
    QStringList tables;

    tables << "luettu_emit_rasti"
           << "luettu_emit"
           << "rasti"
           << "valiaika"
           << "tulos"
           << "tulos_tila"
           << "sarja"
           << "emit"
           << "kilpailija"
           << "tapahtuma"
    ;

    query.prepare("DROP TABLE IF EXISTS ?");

    foreach (QString table, tables) {
        query.addBindValue(table);

        SQL_EXEC(query,);
    }*/
}

void Tietokanta::insertData()
{
    QSqlQuery query;

    QStringList tulos_tila;

    tulos_tila << _("Avoin")
               << _("Hyväksytty")
               << _("Hylätty")
               << _("Keskeyttänyt")
    ;

    query.prepare("INSERT INTO tulos_tila (nimi) VALUES (?)");

    foreach (QString tila, tulos_tila) {
        query.addBindValue(tila);

        SQL_EXEC(query,);
    }

    query.prepare("INSERT INTO sarja (tapahtuma, nimi) VALUES (?, ?)");

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(_("Tyhjä"));

    SQL_EXEC(query,);
}

void Tietokanta::luoKilpailijatietokanta(const QString &fn)
{
#ifdef USE_MYSQL
    // FIXME tämä täytyy tehdä eri tavalla kuin SQLite:n kanssa.
    QSqlDatabase kilpdat = QSqlDatabase::addDatabase("QSQLITE", "kilpdat");

    kilpdat.setDatabaseName(fn);

    if (!kilpdat.open()) {
        // FIXME Error
        return;
    }

    kilpdat.close();

#else
    QSqlQuery query;

    query.prepare("ATTACH DATABASE ? AS kilpdat");

    query.addBindValue(fn);

    SQL_EXEC(query,);

    QSqlDatabase::database().transaction();
    {
        query.prepare("CREATE TABLE kilpdat.kilpailija AS SELECT * FROM kilpailija");

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE kilpdat.emit AS SELECT * FROM emit");

        SQL_EXEC(query,);
    }
    QSqlDatabase::database().commit();

    query.prepare("DETACH kilpdat");

    SQL_EXEC(query,);
#endif
}

bool Tietokanta::tuoKilpailijatietokanta(const QString &fn)
{
#ifdef USE_MYSQL
    // FIXME tämä täytyy tehdä eri tavalla kuin SQLite:n kanssa.
    QSqlDatabase kilpdat = QSqlDatabase::addDatabase("QSQLITE", "kilpdat");

    kilpdat.setDatabaseName(fn);

    if (!kilpdat.open()) {
        // FIXME Error
        return false;
    }

    kilpdat.close();

#else
    QSqlQuery query;

    query.prepare("ATTACH DATABASE ? AS kilpdat");

    query.addBindValue(fn);

    SQL_EXEC(query, false);

    QSqlDatabase::database().transaction();
    {
        query.prepare("INSERT OR REPLACE INTO kilpailija SELECT * FROM kilpdat.kilpailija");

        SQL_EXEC(query, false);

        query.prepare("INSERT OR REPLACE INTO emit SELECT * FROM kilpdat.emit");

        SQL_EXEC(query, false);
    }
    QSqlDatabase::database().commit();

    query.prepare("DETACH kilpdat");

    SQL_EXEC(query, false);
#endif

    return true;
}

