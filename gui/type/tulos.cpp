#include "tulos.h"

Tulos::Tulos(const QString &sarja, int sija, const QString &kilpailija, int tila, const QTime &aika, const QList<Valiaika>& valiajat) :
    m_sarja(sarja),
    m_sija(sija),
    m_kilpailija(kilpailija),
    m_tila(tila),
    m_aika(aika),
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
                "  k.nimi AS kilpailija,\n"
                "  t.tila,\n"
                "  t.aika,\n"
                "  t.tila = 2 AS hyvaksytty\n"
                "FROM tulos AS t\n"
                "  JOIN kilpailija AS k ON k.id = t.kilpailija\n"
                "WHERE t.tapahtuma = ?\n"
                "  AND t.sarja = ?\n"
                "ORDER BY hyvaksytty DESC,\n"
                "         t.aika ASC\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(sarja->getId());

    SQL_EXEC(query, tulokset);

    int sija = 1;

    while (query.next()) {
        QSqlRecord r = query.record();

        tulokset.append(Tulos(
                            sarja->getNimi(),
                            sija,
                            r.value("kilpailija").toString(),
                            r.value("tila").toInt(),
                            r.value("aika").toTime(),
                            Valiaika::haeValiajat(r.value("id"))
        ));

        sija++;
    }

    return tulokset;
}
