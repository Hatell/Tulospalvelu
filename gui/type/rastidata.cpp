#include "rastidata.h"

RastiData::RastiData(int rasti, int aika) :
    m_rasti(rasti),
    m_aika(aika)
{
}

QList<RastiData> RastiData::luettuRasit(const QVariant &luettuEmitId)
{
    QList<RastiData> rastit;

    QSqlQuery query;

    query.prepare("SELECT * FROM luettu_emit_rasti WHERE luettu_emit = ? ORDER BY numero ASC");

    query.addBindValue(luettuEmitId);

    SQL_EXEC(query, rastit);

    while (query.next()) {
        QSqlRecord r = query.record();

        rastit.append(RastiData(r.value("koodi").toInt(), r.value("aika").toInt()));
    }

    return rastit;
}
