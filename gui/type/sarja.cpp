#include "sarja.h"

Sarja::Sarja(QObject *parent, const QVariant &id, const QString &nimi, const QList<Rasti> rastit) :
    QObject(parent),
    m_id(id),
    m_nimi(nimi),
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

        sarjat.append(new Sarja(parent, r.value("id"), r.value("nimi").toString(), Rasti::haeRastit(r.value("id"))));
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

        return new Sarja(parent, id, r.value("nimi").toString(), Rasti::haeRastit(id));
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
