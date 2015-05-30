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
    explicit Tulos(int id, const QString& sarja, int sija, const QString &_emit, const QString& kilpailija, int tila, const QTime& aika, const QDateTime& maaliaika, const QList<Valiaika>& valiajat);

    static QList<Tulos> haeTulokset(const Sarja* sarja);

    enum Tila {
        Avoin = 1,
        Hyvaksytty = 2,
        DNF = 3
    };

//private:
    int m_id;               // Tietokanta tunniste?, FIXME miksi ei ole QVariant?
    QString m_sarja;        // Sarjan nimi
    int m_sija;             // Sijoitus
    QString m_emit;         // Emitnumero
    QString m_kilpailija;   // Kilpailija nimi
    int m_tila;             // FIXME miksi ei käytetä Tila m_tila?
    QTime m_aika;           // Tulosaika
    QDateTime m_maaliaika;  // Maaliin saapumisaika

    QList<Valiaika> m_valiajat; // Väliajat
};

#endif // TULOS_H
