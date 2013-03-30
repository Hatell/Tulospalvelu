#ifndef TULOS_H
#define TULOS_H

#include <QtCore>

#include "type/tapahtuma.h"
#include "type/valiaika.h"
#include "type/sarja.h"

#include "makrot.h"

class Tulos
{
public:
    explicit Tulos(int id, const QString& sarja, int sija, const QString& kilpailija, int tila, const QTime& aika, const QDateTime& maaliaika, const QList<Valiaika>& valiajat);

    static QList<Tulos> haeTulokset(const Sarja* sarja);

    enum Tila {
        Avoin = 1,
        Hyvaksytty = 2,
        DNF = 3
    };

//private:
    int m_id;
    QString m_sarja;
    int m_sija;
    QString m_kilpailija;
    int m_tila;
    QTime m_aika;
    QDateTime m_maaliaika;

    QList<Valiaika> m_valiajat;
};

#endif // TULOS_H
