#ifndef RASTI_H
#define RASTI_H

#include <QtCore>
#include <QtSql>

#include "makrot.h"

class Sarja;

class Rasti
{
public:
    explicit Rasti(const QVariant& id, int numero, const QList<int>& koodit, bool data = false);

    QVariant getId() const;
    int getNumero() const;

    int getKoodi(int i = 0) const;
    bool sisaltaa(int koodi) const;

    void setNumero(const QVariant& numero);
    void setKoodi(const QVariant& koodi);

    bool dbUpdate() const;
    bool dbDelete() const;
    static Rasti dbInsert(const Sarja* sarja, int numero = 0, int koodi = 0);

    static QList<Rasti> haeRastit(const QVariant &sarjaId);

    // Palauttaa jokaista rastia ja koodi paria kohden yhden Rasti:n
    // Eli ei grouppaa rastinumeron perusteella. Tarvitaan RataModel:n käytössä
    static QList<Rasti> haeRastitData(const QVariant &sarjaId);
private:
    QVariant m_id;          // Tietokanta tunniste
    int m_numero;           // Rastin järjestysnumero
    QList<int> m_koodit;    // Rastikoodit
    bool m_data;            // FIXME Kirjoita selitys, Liittyy päivitettävyyteen.
};

#endif // RASTI_H
