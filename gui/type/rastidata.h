#ifndef RASTIDATA_H
#define RASTIDATA_H

#include <QtCore>
#include <QtSql>

#include "makrot.h"

class RastiData
{
public:
    explicit RastiData(int rasti, int aika);

    int m_rasti;
    int m_aika;

    static QList<RastiData> luettuRasit(const QVariant& luettuEmitId);
};

#endif // RASTIDATA_H
