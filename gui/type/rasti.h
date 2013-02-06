#ifndef RASTI_H
#define RASTI_H

#include <QtCore>
#include <QtSql>

#include "makrot.h"

class Rasti
{
public:
    explicit Rasti(const QVariant& id, int numero, const QList<int>& koodit);

    QVariant getId() const;
    int getNumero() const;
    bool sisaltaa(int koodi) const;

    static int maaliKoodi();
    static QList<Rasti> haeRastit(const QVariant &sarjaId);

private:
    QVariant m_id;
    int m_numero;
    QList<int> m_koodit;

};

#endif // RASTI_H
