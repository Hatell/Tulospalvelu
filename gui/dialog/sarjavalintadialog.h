#ifndef SARJAVALINTADIALOG_H
#define SARJAVALINTADIALOG_H

#include <QtGui>

#include "type/sarja.h"

#include "makrot.h"

namespace Ui {
    class SarjaValintaDialog;
}

class SarjaValintaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SarjaValintaDialog(QWidget *parent, QList< Sarja* > sarjat);
    ~SarjaValintaDialog();

    Sarja* getSarja() const;

private:
    Ui::SarjaValintaDialog *ui;

    QList<Sarja*> m_sarjat;
};

#endif // SARJAVALINTADIALOG_H
