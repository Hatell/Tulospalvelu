#ifndef TAPAHTUMA_H
#define TAPAHTUMA_H

#include <QtCore>
#include <QtSql>

#include "makrot.h"

class Tapahtuma : public QObject
{
    Q_OBJECT
public:
    explicit Tapahtuma(QObject *parent, int id);

    int id() const;
    QString nimi() const;


    static bool valitseTapahtuma(int id);
    static const Tapahtuma* tapahtuma();

    static void luoUusiTapahtuma(const QString& nimi);
signals:

public slots:

private:
    int m_id;
    QString m_nimi;

    static Tapahtuma *ms_tapahtuma;
};

#endif // TAPAHTUMA_H
