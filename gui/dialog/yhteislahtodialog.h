#ifndef YHTEISLAHTODIALOG_H
#define YHTEISLAHTODIALOG_H

#include <QtGui>
#include <QtSql>

#include "type/tapahtuma.h"
#include "type/sarja.h"

#include "makrot.h"

namespace Ui {
class YhteislahtoDialog;
}

struct UusiTulos {
    UusiTulos(const QVariant& a, const QTime& b): valiaika(a), aika(b) {}
    QVariant valiaika;
    QTime aika;
};

class YhteislahtoDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit YhteislahtoDialog(QWidget *parent);
    ~YhteislahtoDialog();
    
    QDateTime getLahtoaika() const;
    QVariant getSarjaId() const;

    static bool muutaYhteislahdoksi(const QVariant& sarja_id, const QDateTime& lahtoaika);

private:
    Ui::YhteislahtoDialog *ui;

    QSqlQueryModel *m_sarjaModel;

    void sqlSarja();
    void sqlLahtoaika();
};

#endif // YHTEISLAHTODIALOG_H
