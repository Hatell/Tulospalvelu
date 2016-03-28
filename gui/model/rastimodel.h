#ifndef RASTIMODEL_H
#define RASTIMODEL_H

#include <QtWidgets>

#include "model/ratamodel.h"

#include "makrot.h"
class RastiModel : public RataModel
{
    Q_OBJECT
public:
    explicit RastiModel(QObject *parent);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

};

#endif // RASTIMODEL_H
