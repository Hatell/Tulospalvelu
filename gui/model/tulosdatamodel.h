#ifndef TULOSDATAMODEL_H
#define TULOSDATAMODEL_H

#include <QtCore>

#include "model/emitdatamodel.h"

#include "type/sarja.h"
#include "type/rasti.h"
#include "type/rastidata.h"

#include "makrot.h"

class Data
{
public:
    Data(const QVariant& _a, const QVariant& _b, const QVariant& _c) : a(_a), b(_b), c(_c) {}

    QVariant a;
    QVariant b;
    QVariant c;
};

class TulosDataModel : public EmitDataModel
{
    Q_OBJECT
public:
    explicit TulosDataModel(QObject *parent, QString numero, int vuosi, int kuukausi, QList<RastiData> rastit, const Sarja* sarja = 0);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void setSarja(const Sarja* sarja);

    QList<Data> getValiajat() const;

private:
    QList<Data> m_data;
    int m_haettu;
    int m_haettuLaite;

};

#endif // TULOSDATAMODEL_H
