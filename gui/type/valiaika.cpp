#include "valiaika.h"

Valiaika::Valiaika(const QVariant& id, int numero, int koodi, const QTime &aika, int sija) :
    m_id(id),
    m_numero(numero),
    m_koodi(koodi),
    m_aika(aika),
    m_sija(sija)
{
}

QList<Valiaika> Valiaika::haeValiajat(const QVariant &tulosId)
{
    QList<Valiaika> valiajat;

    QSqlQuery query;

    query.prepare("SELECT * FROM valiaika WHERE tulos = ? ORDER BY numero ASC");

    query.addBindValue(tulosId);

    SQL_EXEC(query, valiajat);

    while (query.next()) {
        QSqlRecord r = query.record();

        valiajat.append(Valiaika(r.value("id"), r.value("numero").toInt(), r.value("koodi").toInt(), r.value("aika").toTime(), -1));
    }

    return valiajat;
}

QList<Valiaika> Valiaika::haeRastiValiajat(const QVariant &rastiId)
{
    QList<Valiaika> valiajat;

    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  v.*\n"
                "FROM valiaika AS v\n"
                "  JOIN tulos AS t ON t.id = v.tulos\n"
                "  JOIN rasti AS r ON r.sarja = t.sarja\n"
                "WHERE t.tapahtuma = ?\n"
                "  AND t.tila = 2\n"
                "  AND v.koodi = r.koodi\n"
                "  AND v.numero >= r.numero\n"
                "  AND r.id = ?\n"
                "ORDER BY v.aika ASC\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(rastiId);

    SQL_EXEC(query, valiajat);

    int sija = 1;

    while (query.next()) {
        QSqlRecord r = query.record();

        valiajat.append(Valiaika(r.value("id"), r.value("numero").toInt(), r.value("koodi").toInt(), r.value("aika").toTime(), sija));

        sija++;
    }

    return valiajat;
}

QList<Valiaika> Valiaika::karsiYlimaaraiset(const QList<Valiaika> &valiajat, const QList<Rasti> &rastit)
{
    QList<Valiaika> karsitut;

    foreach (Rasti r, rastit) {
        foreach (Valiaika v, valiajat) {
            if (r.sisaltaa(v.m_koodi) && r.getNumero() <= v.m_numero) {
                v.m_numero = r.getNumero(); // Jotta numerointi täsmää

                karsitut.append(v);
                break;
            }
        }
    }

    return karsitut;
}
