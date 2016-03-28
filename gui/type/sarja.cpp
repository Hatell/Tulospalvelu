#include "sarja.h"

Sarja::Sarja(QObject *parent, const QVariant &id, const QString &nimi, int sakkoaika, const QVariant& yhteislahto, const QList<Rasti> rastit, bool data) :
    QObject(parent),
    m_id(id),
    m_nimi(nimi),
    m_sakkoaika(sakkoaika),
    m_yhteislahto(yhteislahto),
    m_rastit(rastit),
    m_data(data)
{
}

QList<Sarja*> Sarja::haeSarjat(QObject *parent, const Tapahtuma *tapahtuma)
{
    QList<Sarja*> sarjat;

    QSqlQuery query;

    query.prepare("SELECT * FROM sarja WHERE tapahtuma = ?");

    query.addBindValue(tapahtuma->id());

    SQL_EXEC(query, sarjat);

    while (query.next()) {
        QSqlRecord r = query.record();

        sarjat.append(new Sarja(parent, r.value("id"), r.value("nimi").toString(), r.value("sakkoaika").toInt(), r.value("yhteislahto"), Rasti::haeRastit(r.value("id"))));
    }

    return sarjat;
}

QList<Sarja*> Sarja::haeSarjatData(QObject *parent, const Tapahtuma *tapahtuma)
{
    QList<Sarja*> sarjat;

    QSqlQuery query;

    query.prepare("SELECT * FROM sarja WHERE tapahtuma = ?");

    query.addBindValue(tapahtuma->id());

    SQL_EXEC(query, sarjat);

    while (query.next()) {
        QSqlRecord r = query.record();

        sarjat.append(new Sarja(parent, r.value("id"), r.value("nimi").toString(), r.value("sakkoaika").toInt(), r.value("yhteislahto"), Rasti::haeRastitData(r.value("id")), true));
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

        return new Sarja(parent, id, r.value("nimi").toString(), r.value("sakkoaika").toInt(), r.value("yhteislahto"), Rasti::haeRastit(id));
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

bool Sarja::isYhteislahto() const
{
    return !m_yhteislahto.isNull();
}

QVariant Sarja::getYhteislahto() const
{
    return m_yhteislahto;
}

bool Sarja::isSakkoaika() const
{
    return m_sakkoaika != -1;
}

int Sarja::getSakkoaika() const
{
    return isSakkoaika() ? m_sakkoaika : 0;
}

void Sarja::setNimi(const QVariant &nimi)
{
    if (!m_data) {
        return;
    }

    m_nimi = nimi.toString();
}

void Sarja::setSakkoaika(const QVariant &sakkoaika)
{
    if (!m_data) {
        return;
    }

    m_sakkoaika = sakkoaika.toInt();
}

void Sarja::setYhteislahto(const QVariant &yhteislahto)
{
    if (!m_data) {
        return;
    }

    if (yhteislahto.toDateTime().isValid()) {
        QDateTime a = yhteislahto.toDateTime();
        a.setTime(QTime(
            a.time().hour(),
            a.time().minute(),
            a.time().second()
        ));
        m_yhteislahto = a;
    } else {
        m_yhteislahto = QVariant();
    }
}

bool Sarja::dbUpdate() const
{
    if (!m_data) {
        return false;
    }

    QSqlQuery query;

    query.prepare("UPDATE sarja SET nimi = ?, sakkoaika = ?, yhteislahto = ? WHERE id = ?");

    query.addBindValue(m_nimi);
    query.addBindValue(m_sakkoaika);
    query.addBindValue(m_yhteislahto);
    query.addBindValue(m_id);

    SQL_EXEC(query, false);

    return true;
}

void Sarja::replaceRasti(int index, const Rasti &rasti)
{
    m_rastit.replace(index, rasti);
}

Sarja * Sarja::dbInsert(QObject *parent, const Tapahtuma *tapahtuma)
{
    QSqlDatabase::database().transaction();

    QSqlQuery query;

    QString nimi = _("Uusi rata");

    query.prepare("INSERT INTO sarja (tapahtuma, nimi) VALUES (?, ?)");

    query.addBindValue(tapahtuma->id());
    query.addBindValue(nimi);

    SQL_EXEC(query, 0);

    QVariant id = query.lastInsertId();

    QSqlDatabase::database().commit();

    return new Sarja(parent, id, nimi, -1, QVariant(), QList<Rasti>(), true);
}

void Sarja::insertRasti(int index, const Rasti &rasti)
{
    m_rastit.insert(index, rasti);
}

bool Sarja::dbDelete() const
{
    if (!m_data) {
        return false;
    }

    QSqlDatabase::database().transaction();

    QSqlQuery query;

    query.prepare("DELETE FROM rasti WHERE sarja = ?");

    query.addBindValue(m_id);

    SQL_EXEC(query, false);

    query.prepare("DELETE FROM sarja WHERE id = ?");

    query.addBindValue(m_id);

    SQL_EXEC(query, false);

    QSqlDatabase::database().commit();

    return true;
}

void Sarja::removeRasti(int index)
{
    m_rastit.removeAt(index);
}
