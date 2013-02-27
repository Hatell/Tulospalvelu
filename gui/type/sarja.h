#ifndef SARJA_H
#define SARJA_H

#include <QtCore>
#include <QtSql>

#include "type/rasti.h"
#include "type/tapahtuma.h"

#include "makrot.h"

class Sarja : public QObject
{
    Q_OBJECT
public:
    explicit Sarja(QObject *parent,const QVariant& id, const QString& nimi, const QList<Rasti> rastit);

    static Sarja* haeSarja(QObject *parent, const QVariant &id);

    static QList<Sarja*> haeSarjat(QObject *parent);

    QVariant getId() const;
    QString getNimi() const;
    QList<Rasti> getRastit() const;
    Rasti getMaalirasti() const;

private:
    QVariant m_id;

    QString m_nimi;

    QList<Rasti> m_rastit;

    // FIXME näitä ei tarvita kun jatkossa ei käytetä kopiointia vaan osoittimia.
//    Sarja(const Sarja &s);
//    Sarja& operator =(const Sarja &s);
};

#endif // SARJA_H
