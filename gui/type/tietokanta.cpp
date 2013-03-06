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
        "   maaliaika DATETIME NOT NULL,"
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

void Tietokanta::vieTulokset(const Tapahtuma *tapahtuma, const QString &fileName)
{
#ifdef USE_MYSQL
    // FIXME tee loppuun
#else
    QSqlQuery query;

    query.prepare("ATTACH DATABASE ? AS tulosdat");

    query.addBindValue(fileName);

    SQL_EXEC(query,);

    QSqlDatabase::database().transaction();
    {
        query.prepare("CREATE TABLE tulosdat.kilpailija AS SELECT k.* FROM kilpailija AS k WHERE k.id IN (SELECT t.kilpailija FROM tulos AS t WHERE t.tapahtuma = ?)");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.emit AS SELECT e.* FROM emit AS e WHERE e.id IN (SELECT t.emit FROM tulos AS t WHERE t.tapahtuma = ?)");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.tapahtuma AS SELECT t.* FROM tapahtuma AS t WHERE t.id = ?");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.sarja AS SELECT s.* FROM sarja AS s WHERE s.tapahtuma = ?");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.tulos_tila AS SELECT t.* FROM tulos_tila AS t");

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.tulos AS SELECT t.* FROM tulos AS t WHERE t.tapahtuma = ?");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.valiaika AS SELECT v.* FROM valiaika AS v WHERE v.tulos IN (SELECT t.id FROM tulos AS t WHERE t.tapahtuma = ?)");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.luettu_emit AS SELECT l.* FROM luettu_emit AS l WHERE l.tapahtuma = ?");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);

        query.prepare("CREATE TABLE tulosdat.luettu_emit_rasti AS SELECT lr.* FROM luettu_emit_rasti AS lr WHERE lr.luettu_emit IN (SELECT l.id FROM luettu_emit AS l WHERE l.tapahtuma = ?)");

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query,);
    }
    QSqlDatabase::database().commit();

    query.prepare("DETACH tulosdat");

    SQL_EXEC(query,);
#endif

}

bool Tietokanta::tuoTulokset(const Tapahtuma *tapahtuma, const QString &fileName)
{
#ifdef USE_MYSQL
    // FIXME tee
#else
    QSqlQuery query;
    QSqlQuery queryValiaikaAppend;

    QSqlQuery queryAppend;

    QSqlQuery queryLuettuAppend;
    QSqlQuery queryLuettuRastiAppend;

    query.prepare("ATTACH DATABASE ? AS tulosdat");

    query.addBindValue(fileName);

    SQL_EXEC(query, false);

    QSqlDatabase::database().transaction();
    {
        // Tarkistetaan että tulosdat sisältää vain yhden tapahtuman
        query.prepare(
                    "SELECT\n"
                    "  COUNT(*)\n"
                    "FROM tulosdat.tapahtuma\n"
        );

        SQL_EXEC(query, false);

        query.next();

        if (query.value(0).toInt() != 1) {
            // FIXME tämä ei ole hyvä tapa, keksi parempi. Esim throw error tai vastaavaa
            INFO(0, _("%1 ei sisällä vain yksiä tuloksia. Ei voida tuoda tuloksia.").arg(fileName));
            return false;
        }

        // Tarkistetaan löytyykö tapahtumaa
        query.prepare(
                    "SELECT\n"
                    "  COUNT(*)\n"
                    "FROM tulosdat.tapahtuma AS t\n"
                    "WHERE t.nimi = ?\n"
        );

        query.addBindValue(tapahtuma->nimi());

        SQL_EXEC(query, false);

        query.next();

        if (query.value(0).toInt() != 1) {
            // FIXME katso edellinen FIXME
            INFO(0, _("%1 ei sisällä tätä tapahtumaa (%2).").arg(fileName, tapahtuma->nimi()));
            return false;
        }

        // Tarkistetaan että tapahtuman sarjat on olemassa
        query.prepare(
                    "SELECT\n"
                    "  COUNT(*)\n"
                    "FROM tulosdat.sarja AS a\n"
                    "WHERE a.nimi NOT IN (\n"
                    "  SELECT\n"
                    "    b.nimi\n"
                    "  FROM sarja AS b\n"
                    "  WHERE b.tapahtuma = ?\n"
                    ")\n"
        );

        query.addBindValue(tapahtuma->id());

        SQL_EXEC(query, false);

        query.next();

        if (query.value(0).toInt() != 0) {
            // FIXME katso edellinen FIXME
            INFO(0, _("%1:n sisältämä tapahtumaa ei löydy tästä tulostietokannasta.").arg(fileName));
            return false;
        }


        // Emit
        // Nämä voidaan lisätä ja ohitetaan emit taulussa olevat emitit
        query.prepare("INSERT OR IGNORE INTO emit SELECT * FROM tulosdat.emit");

        SQL_EXEC(query, false);


        // Kilpailija taulun päivitys, lisätään kilpailijat joita ei ole.
        query.prepare("SELECT nimi FROM tulosdat.kilpailija WHERE nimi NOT IN (SELECT nimi FROM kilpailija)");

        SQL_EXEC(query, false);

        queryAppend.prepare("INSERT INTO kilpailija (nimi) VALUES (?)");

        while (query.next()) {
            QSqlRecord r = query.record();

            queryAppend.addBindValue(r.value("nimi"));

            SQL_EXEC(queryAppend, false);
        }


        // TulosTila taulun päivitys, lisätään tilat joita ei ole.
        query.prepare("SELECT nimi FROM tulosdat.tulos_tila WHERE nimi NOT IN (SELECT nimi FROM tulos_tila)");

        SQL_EXEC(query, false);

        queryAppend.prepare("INSERT INTO tulos_tila (nimi) VALUES (?)");

        while (query.next()) {
            QSqlRecord r = query.record();

            queryAppend.addBindValue(r.value("nimi"));

            SQL_EXEC(queryAppend, false);
        }


        // Kasataan tulos "selkokieliseksi" ja yksitellen puretaan ja tarvittaessa lisätään kilpailija
        query.prepare(
                    "SELECT\n"
                    "  t.id AS id,\n"
                    "  ta.nimi AS tapahtuma,\n"
                    "  t.emit AS emit,\n"
                    "  k.nimi AS kilpailija,\n"
                    "  s.nimi AS sarja,\n"
                    "  tt.nimi AS tila,\n"
                    "  t.aika AS aika\n"
                    "FROM tulosdat.tulos AS t\n"
                    "  JOIN tulosdat.kilpailija AS k ON k.id = t.kilpailija\n"
                    "  JOIN tulosdat.sarja AS s ON s.id = t.sarja\n"
                    "  JOIN tulosdat.tulos_tila AS tt ON tt.id = t.tila\n"
                    "  JOIN tulosdat.tapahtuma AS ta ON ta.id = t.tapahtuma\n"
        );

        queryValiaikaAppend.prepare(
                    "INSERT INTO valiaika (\n"
                    "  tulos,\n"
                    "  numero,\n"
                    "  koodi,\n"
                    "  aika\n"
                    ") SELECT\n"
                    "  ?,\n"
                    "  v.numero,\n"
                    "  v.koodi,\n"
                    "  v.aika\n"
                    "FROM tulosdat.valiaika AS v\n"
                    "WHERE v.tulos = ?\n"
        );

        queryLuettuAppend.prepare(
                    "INSERT INTO luettu_emit (\n"
                    "  tapahtuma,\n"
                    "  emit,\n"
                    "  luettu,\n"
                    "  tulos\n"
                    ") SELECT\n"
                    "  ?,\n"
                    "  l.emit,\n"
                    "  l.luettu,\n"
                    "  ?\n"
                    "FROM tulosdat.luettu_emit AS l\n"
                    "WHERE l.tulos = ?\n"
        );

        queryLuettuRastiAppend.prepare(
                    "INSERT INTO luettu_emit_rasti (\n"
                    "  luettu_emit,\n"
                    "  numero,\n"
                    "  koodi,\n"
                    "  aika\n"
                    ") SELECT\n"
                    "  ?,\n"
                    "  lr.numero,\n"
                    "  lr.koodi,\n"
                    "  lr.aika\n"
                    "FROM tulosdat.luettu_emit_rasti AS lr\n"
                    "  JOIN tulosdat.luettu_emit AS l ON l.id = lr.luettu_emit\n"
                    "WHERE l.tulos = ?\n"
        );

        queryAppend.prepare(
                    "INSERT INTO tulos (\n"
                    "  tapahtuma,\n"
                    "  emit,\n"
                    "  kilpailija,\n"
                    "  sarja,\n"
                    "  tila,\n"
                    "  aika\n"
                    ") SELECT\n"
                    "  ta.id,\n"
                    "  t.emit,\n"
                    "  k.id,\n"
                    "  s.id,\n"
                    "  tt.id,\n"
                    "  t.aika\n"
                    "FROM (\n"
                    "  SELECT\n"
                    "    t.id AS id,\n"
                    "    ta.nimi AS tapahtuma,\n"
                    "    t.emit AS emit,\n"
                    "    k.nimi AS kilpailija,\n"
                    "    s.nimi AS sarja,\n"
                    "    tt.nimi AS tila,\n"
                    "    t.aika AS aika\n"
                    "  FROM tulosdat.tulos AS t\n"
                    "    JOIN tulosdat.kilpailija AS k ON k.id = t.kilpailija\n"
                    "    JOIN tulosdat.sarja AS s ON s.id = t.sarja\n"
                    "    JOIN tulosdat.tulos_tila AS tt ON tt.id = t.tila\n"
                    "    JOIN tulosdat.tapahtuma AS ta ON ta.id = t.tapahtuma\n"
                    "  WHERE t.id = ?\n"
                    ") AS t\n"
                    "  JOIN tapahtuma AS ta ON ta.nimi = t.tapahtuma\n"
                    "  JOIN kilpailija AS k ON k.nimi = t.kilpailija\n"
                    "  JOIN tulos_tila AS tt ON tt.nimi = t.tila\n"
                    "  JOIN sarja AS s ON s.nimi = t.sarja\n"
                    "                 AND s.tapahtuma = ta.id\n"
        );

        SQL_EXEC(query, false);

        while (query.next()) {
            QSqlRecord r = query.record();
            QVariant newTulosId;
            QVariant newLuettuEmitId;

            queryAppend.addBindValue(r.value("id"));

            SQL_EXEC(queryAppend, false);

            newTulosId = queryAppend.lastInsertId();

            queryValiaikaAppend.addBindValue(newTulosId);
            queryValiaikaAppend.addBindValue(r.value("id"));

            SQL_EXEC(queryValiaikaAppend, false);

            queryLuettuAppend.addBindValue(tapahtuma->id());
            queryLuettuAppend.addBindValue(newTulosId);
            queryLuettuAppend.addBindValue(r.value("id"));

            SQL_EXEC(queryLuettuAppend, false);

            newLuettuEmitId = queryLuettuAppend.lastInsertId();

            queryLuettuRastiAppend.addBindValue(newLuettuEmitId);
            queryLuettuRastiAppend.addBindValue(r.value("id"));

            SQL_EXEC(queryLuettuRastiAppend, false);
        }
    }
    QSqlDatabase::database().commit();

    query.prepare("DETACH tulosdat");

    SQL_EXEC(query, false);
#endif

    return true;
}

