#include "tapahtuma.h"

Tapahtuma * Tapahtuma::ms_tapahtuma = new Tapahtuma(0, 0);

Tapahtuma::Tapahtuma(QObject *parent, int id) :
    QObject(parent),
    m_id(id),
    m_nimi()
{
}


int Tapahtuma::id() const
{
    return m_id;
}

QString Tapahtuma::nimi() const
{
    return m_nimi;
}

void Tapahtuma::luoUusiTapahtuma(const QString &nimi)
{
    QSqlQuery query;

    query.prepare("INSERT INTO tapahtuma (nimi) VALUES (?)");

    query.addBindValue(nimi);

    SQL_EXEC(query,);

    ms_tapahtuma->m_id = query.lastInsertId().toInt();
    ms_tapahtuma->m_nimi = nimi;
}

const Tapahtuma* Tapahtuma::tapahtuma()
{
    if (ms_tapahtuma->m_nimi.isNull()) {
        Tapahtuma::valitseTapahtuma(ms_tapahtuma->m_id);
    }

    return ms_tapahtuma;
}

bool Tapahtuma::valitseTapahtuma(int id)
{
    ms_tapahtuma->m_id = id;
    ms_tapahtuma->m_nimi = QString();

    QSqlQuery query;

    query.prepare("SELECT nimi FROM tapahtuma WHERE id = ?");

    query.addBindValue(ms_tapahtuma->m_id);

    SQL_EXEC(query, false);

    if (!query.next()) {
        return false;
    }

    ms_tapahtuma->m_nimi = query.value(0).toString();

    return true;
}
