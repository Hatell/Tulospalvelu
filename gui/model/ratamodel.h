#ifndef RATAMODEL_H
#define RATAMODEL_H

#include <QtGui>
#include <QtSql>

#include "type/tapahtuma.h"
#include "type/sarja.h"
#include "type/rasti.h"

#include "makrot.h"

class RataModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit RataModel(QObject *parent);
    explicit RataModel(QObject *parent, const Tapahtuma* tapahtuma);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QModelIndex parent(const QModelIndex &child = QModelIndex()) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRow(int row, const QModelIndex &parent);
signals:

public slots:

private:
    const Tapahtuma* m_tapahtuma;

    QList<Sarja*> m_sarjat;
};

#endif // RATAMODEL_H
