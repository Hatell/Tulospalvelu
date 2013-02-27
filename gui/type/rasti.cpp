#include "rasti.h"

Rasti::Rasti(const QVariant& id, int numero, const QList<int> &koodit) :
    m_id(id),
    m_numero(numero),
    m_koodit(koodit)
{
}

QList<Rasti> Rasti::haeRastit(const QVariant &sarjaId)
{
    QList<Rasti> rastit;
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  id,\n"
                "  numero,\n"
                "  GROUP_CONCAT(koodi) AS koodi\n"
                "FROM rasti\n"
                "WHERE sarja = ?\n"
                "GROUP BY numero\n"
                "ORDER BY numero ASC\n"
    );

    query.addBindValue(sarjaId);

    SQL_EXEC(query, rastit);

    while (query.next()) {
        QSqlRecord r = query.record();

        QList<int> koodit;

        foreach (QString koodi, r.value("koodi").toString().split(",")) {
            koodit << koodi.toInt();
        }

        rastit.append(Rasti(r.value("id"), r.value("numero").toInt(), koodit));
    }

    return rastit;
}

/*
int Rasti::maaliKoodi()
{
    // FIXME hae asetuksista
    return 100;
}*/

QVariant Rasti::getId() const
{
    return m_id;
}

int Rasti::getNumero() const
{
    return m_numero;
}

bool Rasti::sisaltaa(int koodi) const
{
    return m_koodit.contains(koodi);
}
