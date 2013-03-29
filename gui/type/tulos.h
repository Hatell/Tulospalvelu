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
    explicit Tulos(const QString& sarja, int sija, const QString& kilpailija, int tila, const QTime& aika, const QList<Valiaika>& valiajat);

    static QList<Tulos> haeTulokset(const Sarja* sarja);

    enum Tila {
        Avoin = 1,
        Hyvaksytty = 2,
        DNF = 3
    };

//private:
    QString m_sarja;
    int m_sija;
    QString m_kilpailija;
    int m_tila;
    QTime m_aika;

    QList<Valiaika> m_valiajat;
};

#endif // TULOS_H
