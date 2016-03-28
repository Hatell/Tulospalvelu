#include "tulosdatamodel.h"

TulosDataModel::TulosDataModel(QObject *parent, QString numero, int vuosi, int kuukausi, QList<RastiData> rastit, const Sarja *sarja) :
    EmitDataModel(parent, numero, vuosi, kuukausi, rastit, 0),
    m_haettu(0),
    m_haettuLaite(0)
{
    foreach (RastiData d, m_rastit) {
        if (d.m_rasti == 250 || d.m_rasti == 254 || d.m_rasti == 99) {
            m_haettuLaite++;
        } else if (d.m_rasti != 0) {
            m_haettu++;
        }
    }

    setSarja(sarja);
}

QVariant TulosDataModel::data(const QModelIndex &index, int role) const
{
    if (!m_sarja) {
        return QVariant();
    }

    if (role == Qt::ForegroundRole || static_cast<int>(index.internalId()) <= -1) {
        return EmitDataModel::data(index, role);
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QVariant();
    }

    Data d = m_data.at(index.row());

    switch (index.column()) {
    case 0:
        return d.a;
    case 1:
        return d.b;
    case 2:
        return QTime(0,0).addSecs(d.c.toInt()).toString("HH:mm:ss") + _(" (%1)").arg(d.c.toString());
    }

    return QVariant();
}

Qt::ItemFlags TulosDataModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

int TulosDataModel::rowCount(const QModelIndex &parent) const
{
    if (!m_sarja) {
        return 0;
    }

    if (parent.isValid()) {
        if (parent.parent().isValid()) {
            return 0;
        }

        return m_data.count();
    }

    return 1;
}

void TulosDataModel::setSarja(const Sarja *sarja)
{
    beginResetModel();
    m_sarja = sarja;

    m_varit.clear();
    m_data.clear();

    if (!m_sarja) {
        return;
    }

    QList<Rasti> rastit = m_sarja->getRastit();

    int data_i = 0;
    int rasti_i = 0;

    while (data_i < m_rastit.count() || rasti_i < rastit.count()) {
        RastiData d(-1, -1);
        if (data_i < m_rastit.count()) {
           d = m_rastit.at(data_i);
        }

        // ohitetaan 0 koodilla olevat rastit
        if (d.m_rasti == 0) {
            data_i++;
            continue;
        }

        Rasti r(QVariant(), -1, QList<int>());
        if (rasti_i < rastit.count()) {
            r = rastit.at(rasti_i);
        }

        // Luetut rastit loppuivat
        if (d.m_rasti == -1 && d.m_aika == -1) {
            if (r.getNumero() != -1) {
                m_data.append(Data(r.getNumero(), _("-"), _("rasti puuttuu")));
                m_varit.append(QColor(Qt::red));
            }
            rasti_i++;
            continue;
        }

        // Lukulaitteiden koodit ja 99 koodi
        // Käsitellään seuraava RastiData ja Rasti ei liiku
        if (d.m_rasti == 250 || d.m_rasti == 254 || d.m_rasti == 99) {
            m_data.append(Data(_("*"), d.m_rasti, d.m_aika));
            m_varit.append(QColor(Qt::blue));
            data_i++;
            continue;
        }

        // Oikein ja oikeassa paikassa haettu rasti
        if (r.sisaltaa(d.m_rasti)) {
            m_data.append(Data(r.getNumero(), d.m_rasti, d.m_aika));
            m_varit.append(QColor(Qt::darkGreen));
            data_i++;
            rasti_i++;
            continue;
        }

        // Tarkistetaan onko haettu rasti radalla
        bool found = false;

        for (int i = rasti_i; i < rastit.count(); i++) {
            Rasti rr = rastit.at(i);
            if (rr.sisaltaa(d.m_rasti)) {
                found = true;
                break;
            }
        }

        if (!found) {
            m_data.append(Data(_("?"), d.m_rasti, d.m_aika));
            m_varit.append(QColor(Qt::gray));
            data_i++;
            continue;
        }

        // Tarkistetaan onko rastia haettu
        found = false;

        for (int i = data_i; i < m_rastit.count(); i++) {
            RastiData dd = m_rastit.at(i);

            if (r.sisaltaa(dd.m_rasti)) {
                found = true;
                break;
            }
        }

        bool last = true;

        // Tarkistetaan ettei rastia ole myöhempänä.
        // Tämä silmukoiden takia
        for (int i = rasti_i + 1; i < rastit.count(); i++) {
            Rasti rr = rastit.at(i);

            if (rr.sisaltaa(r.getKoodi())) {
                last = false;
                break;
            }
        }

        // merkitään harmaaksi rastit, jotka haettiin tässä välissä
        if (found && last) {
            for (; data_i < m_rastit.count(); data_i++) {
                RastiData dd = m_rastit.at(data_i);
                if (r.sisaltaa(dd.m_rasti)) {
                    m_data.append(Data(r.getNumero(), dd.m_rasti, dd.m_aika));
                    m_varit.append(QColor(Qt::darkGreen));
                    break;
                } else {
                    m_data.append(Data(_("?"), dd.m_rasti, dd.m_aika));
                    m_varit.append(QColor(Qt::gray));
                }
            }

            data_i++;
            rasti_i++;

            continue;
        }

        QString tila = _("rasti puuttuu");

        for (int i = data_i; i < m_rastit.count(); i++) {
            RastiData dd = m_rastit.at(i);

            if (r.sisaltaa(dd.m_rasti)) {
                tila = _("rasti väärin");
                break;
            }
        }

        m_data.append(Data(r.getNumero(), _("-"), tila));
        m_varit.append(QColor(Qt::red));

        rasti_i++;
        continue;
    }

    endResetModel();
}

QList<Data> TulosDataModel::getValiajat() const
{
    QList<Data> valiajat;

    if (!m_sarja) {
        return valiajat;
    }

    for (int i = 0; i < m_data.count(); i++) {
        if (m_varit.at(i) == Qt::darkGreen) {
            valiajat << m_data.at(i);
        }
    }

    return valiajat;
}
