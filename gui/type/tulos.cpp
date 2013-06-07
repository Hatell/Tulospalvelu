#include "tulos.h"

Tulos::Tulos(int id, const QString &sarja, int sija, const QString &_emit, const QString &kilpailija, int tila, const QTime &aika, const QDateTime& maaliaika, const QList<Valiaika>& valiajat) :
    m_id(id),
    m_sarja(sarja),
    m_sija(sija),
    m_emit(_emit),
    m_kilpailija(kilpailija),
    m_tila(tila),
    m_aika(aika),
    m_maaliaika(maaliaika),
    m_valiajat(valiajat)
{
}

QList<Tulos> Tulos::haeTulokset(const Sarja* sarja)
{
    QList<Tulos> tulokset;

    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  t.id,\n"
                "  t.emit,\n"
                "  k.nimi AS kilpailija,\n"
                "  t.tila,\n"
                "  t.aika,\n"
                "  t.maaliaika,\n"
                "  t.tila = 2 AS hyvaksytty\n"
                "FROM tulos AS t\n"
                "  JOIN kilpailija AS k ON k.id = t.kilpailija\n"
                "WHERE t.tapahtuma = ?\n"
                "  AND t.sarja = ?\n"
                "  AND NOT t.poistettu\n"
                "ORDER BY hyvaksytty DESC,\n"
                "         t.aika ASC\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(sarja->getId());

    SQL_EXEC(query, tulokset);

    QList<QSqlRecord> res;

    QList<QTime> ajat;

    while (query.next()) {
        QSqlRecord r = query.record();
        res << r;
        if (r.value("hyvaksytty").toBool()) {
            ajat << r.value("aika").toTime();
        }
    }

    foreach (QSqlRecord r, res) {

        int sija = 1;

        foreach (QTime t, ajat) {
            if (t < r.value("aika").toTime()) {
                sija++;
            }
        }

        tulokset.append(Tulos(
                            r.value("id").toInt(),
                            sarja->getNimi(),
                            sija,
                            r.value("emit").toString(),
                            r.value("kilpailija").toString(),
                            r.value("tila").toInt(),
                            r.value("aika").toTime(),
                            r.value("maaliaika").toDateTime(),
                            Valiaika::haeValiajat(r.value("id"))
        ));

        sija++;
    }

    return tulokset;
}
