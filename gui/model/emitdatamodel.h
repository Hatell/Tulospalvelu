#ifndef EMITDATAMODEL_H
#define EMITDATAMODEL_H

#include <QtGui>

#include "type/rastidata.h"
#include "type/sarja.h"
#include "type/rasti.h"

#include "makrot.h"

class EmitDataModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit EmitDataModel(QObject *parent, QString numero = "54321", int vuosi = 12, int kuukausi = 7, QList<RastiData> rastit = QList<RastiData>(), const Sarja* sarja = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QModelIndex parent(const QModelIndex &child = QModelIndex()) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QString getNumero() const;
    int getVuosi() const;
    int getKuukausi() const;
    QList<RastiData> getRastit() const;

    void setSarja(const Sarja* sarja);
    const Sarja* getSarja() const;

private:
    QString m_numero;
    int m_vuosi;
    int m_kuukausi;
    QList<RastiData> m_rastit;
    QList<QColor> m_varit;

    const Sarja *m_sarja;

};

#endif // EMITDATAMODEL_H
