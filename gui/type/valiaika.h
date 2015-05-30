#ifndef VALIAIKA_H
#define VALIAIKA_H

#include <QtCore>
#include <QtSql>

#include "type/tapahtuma.h"

#include "type/sarja.h"
#include "type/rasti.h"

#include "makrot.h"

class Valiaika
{
public:
    Valiaika(const QVariant& id, int numero, int koodi, const QTime& aika, int sija);

    //static QList<Valiaika> karsiYlimaaraiset(const QList<Valiaika>& valiajat, const QList<Rasti>& rastit);

    static QList<Valiaika> haeValiajat(const QVariant& tulosId);

    static QList<Valiaika> haeRastiValiajat(const Sarja* sarja, const Rasti& rasti);
//private:
    QVariant m_id;  // Tietokanta tunniste
    int m_numero;   // Rastin järjestysnumero
    int m_koodi;    // Emitkoodi
    QTime m_aika;   // Aika
    int m_sija;     // Sijoitus

};

#endif // VALIAIKA_H
