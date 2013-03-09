#include "sarja.h"

Sarja::Sarja(QObject *parent, const QVariant &id, const QString &nimi, int sakkoaika, const QList<Rasti> rastit) :
    QObject(parent),
    m_id(id),
    m_nimi(nimi),
    m_sakkoaika(sakkoaika),
    m_rastit(rastit)
{
}

QList<Sarja*> Sarja::haeSarjat(QObject *parent)
{
    QList<Sarja*> sarjat;

    QSqlQuery query;

    query.prepare("SELECT * FROM sarja WHERE tapahtuma = ?");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query, sarjat);

    while (query.next()) {
        QSqlRecord r = query.record();

        sarjat.append(new Sarja(parent, r.value("id"), r.value("nimi").toString(), r.value("sakkoaika").toInt(), Rasti::haeRastit(r.value("id"))));
    }

    return sarjat;
}

Sarja * Sarja::haeSarja(QObject *parent, const QVariant &id)
{
    QSqlQuery query;

    query.prepare("SELECT * FROM sarja WHERE tapahtuma = ? AND id = ?");

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(id);

    SQL_EXEC(query, 0);

    if (query.next()) {
        QSqlRecord r = query.record();

        return new Sarja(parent, id, r.value("nimi").toString(), r.value("sakkoaika").toInt(), Rasti::haeRastit(id));
    }

    return 0;
}

QVariant Sarja::getId() const
{
    return m_id;
}

QString Sarja::getNimi() const
{
    return m_nimi;
}

QList<Rasti> Sarja::getRastit() const
{
    return m_rastit;
}

Rasti Sarja::getMaalirasti() const
{
    if (m_rastit.isEmpty()) {
        return Rasti(QVariant(), 0, QList<int>());
    }

    return m_rastit.last();
}

bool Sarja::isSakkoaika() const
{
    return m_sakkoaika != -1;
}

int Sarja::getSakkoaika() const
{
    return isSakkoaika() ? m_sakkoaika : 0;
}
