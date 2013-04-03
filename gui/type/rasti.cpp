#include "rasti.h"

Rasti::Rasti(const QVariant& id, int numero, const QList<int> &koodit, bool data) :
    m_id(id),
    m_numero(numero),
    m_koodit(koodit),
    m_data(data)
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

int Rasti::getKoodi(int i) const
{
    if (i >= m_koodit.count()) {
        return -1;
    }

    return m_koodit.at(i);
}

QList<Rasti> Rasti::haeRastitData(const QVariant &sarjaId)
{
    QList<Rasti> rastit;
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  id,\n"
                "  numero,\n"
                "  koodi AS koodi\n"
                "FROM rasti\n"
                "WHERE sarja = ?\n"
                "ORDER BY numero ASC\n"
    );

    query.addBindValue(sarjaId);

    SQL_EXEC(query, rastit);

    while (query.next()) {
        QSqlRecord r = query.record();

        rastit.append(Rasti(r.value("id"), r.value("numero").toInt(), QList<int> () << r.value("koodi").toInt(), true));
    }

    return rastit;
}

void Rasti::setNumero(const QVariant &numero)
{
    if (!m_data) {
        return;
    }

    m_numero = numero.toInt();
}

void Rasti::setKoodi(const QVariant &koodi)
{
    if (!m_data) {
        return;
    }

    m_koodit.replace(0, koodi.toInt());
}

bool Rasti::dbUpdate()
{
    if (!m_data) {
        return false;
    }

    QSqlQuery query;

    query.prepare("UPDATE rasti SET numero = ?, koodi = ? WHERE id = ?");

    query.addBindValue(m_numero);
    query.addBindValue(m_koodit.at(0));
    query.addBindValue(m_id);

    SQL_EXEC(query, false);

    return true;
}

