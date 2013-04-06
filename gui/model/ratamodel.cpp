#include "ratamodel.h"

RataModel::RataModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

RataModel::RataModel(QObject *parent, const Tapahtuma *tapahtuma) :
    QAbstractItemModel(parent),
    m_tapahtuma(tapahtuma),
    m_sarjat(Sarja::haeSarjatData(this, tapahtuma))
{
}

Qt::ItemFlags RataModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

int RataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

int RataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.parent().isValid()) {
            return 0;
        }

        return m_sarjat.at(parent.row())->getRastit().count();
    }

    return m_sarjat.count();
}

QVariant RataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return _("ID");
    case 1:
        return _("Nimi");
    case 2:
        return _("Sakkoaika");
    }

    return QVariant();
}

QModelIndex RataModel::parent(const QModelIndex &child) const
{
    if (child.internalId() != -1) {
        return createIndex(m_sarjat.indexOf(static_cast<Sarja*>(child.internalPointer())), 0, -1);
    }

    return QModelIndex();
}

QModelIndex RataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.parent().isValid()) {
            return QModelIndex();
        }

        return createIndex(row, column, m_sarjat.at(parent.row()));
    }

    return createIndex(row, column, -1);
}

QVariant RataModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::EditRole && role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.internalId() == -1) {
        Sarja* s = m_sarjat.at(index.row());

        switch (index.column()) {
        case 0:
            return s->getId();
        case 1:
            return s->getNimi();
        case 2:
            if (s->isSakkoaika()) {
                return s->getSakkoaika();
            }
            return -1;
        }

        return QVariant();
    }

    Rasti r = m_sarjat.at(index.parent().row())->getRastit().at(index.row());

    switch (index.column()) {
    case 0:
        return r.getId();
    case 1:
        return r.getNumero();
    case 2:
        return r.getKoodi();
    }

    return QVariant();
}

bool RataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }

    bool res = false;

    // Rata editointi
    if (index.internalId() == -1) {
        Sarja *s = m_sarjat.at(index.row());

        switch (index.column()) {
        case 0:
            res = false;
            break;
        case 1:
            s->setNimi(value);
            res = s->dbUpdate();
            break;
        case 2:
            s->setSakkoaika(value);
            res = s->dbUpdate();
            break;
        }

    } else {
        Rasti r = m_sarjat.at(index.parent().row())->getRastit().at(index.row());

        switch (index.column()) {
        case 0:
            res = false;
            break;
        case 1:
            r.setNumero(value);
            res = r.dbUpdate();
            break;
        case 2:
            r.setKoodi(value);
            res = r.dbUpdate();
            break;
        }

        m_sarjat.at(index.parent().row())->replaceRasti(index.row(), r);
    }

    if (res) {
        emit dataChanged(index, index);
    }

    return res;
}

bool RataModel::insertRow(int row, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row);

    if (parent.isValid()) {
        // Lisää rasti
        Sarja *s = m_sarjat.at(parent.row());

        s->insertRasti(row, Rasti::dbInsert(s, row + 1));
    } else {
        // Lisää sarja

        m_sarjat.insert(row, Sarja::dbInsert(this, m_tapahtuma));
    }

    endInsertRows();

    return false;
}
