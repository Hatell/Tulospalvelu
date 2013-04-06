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
    explicit Sarja(QObject *parent,const QVariant& id, const QString& nimi, int sakkoaika, const QList<Rasti> rastit, bool data = false);

    static Sarja* haeSarja(QObject *parent, const QVariant &id);

    static QList<Sarja*> haeSarjat(QObject *parent, const Tapahtuma* tapahtuma = Tapahtuma::tapahtuma());
    static QList<Sarja*> haeSarjatData(QObject *parent, const Tapahtuma* tapahtuma = Tapahtuma::tapahtuma());

    QVariant getId() const;
    QString getNimi() const;
    bool isSakkoaika() const;
    int getSakkoaika() const;
    QList<Rasti> getRastit() const;
    Rasti getMaalirasti() const;

    void setNimi(const QVariant& nimi);
    void setSakkoaika(const QVariant& sakkoaika);
    void replaceRasti(int index, const Rasti& rasti);
    void insertRasti(int index, const Rasti& rasti);
    void removeRasti(int index);

    static Sarja* dbInsert(QObject *parent, const Tapahtuma *tapahtuma);
    bool dbUpdate() const;
    bool dbDelete() const;

private:
    QVariant m_id;

    QString m_nimi;
    int m_sakkoaika;

    QList<Rasti> m_rastit;

    bool m_data;
    // FIXME näitä ei tarvita kun jatkossa ei käytetä kopiointia vaan osoittimia.
//    Sarja(const Sarja &s);
//    Sarja& operator =(const Sarja &s);
};

#endif // SARJA_H
