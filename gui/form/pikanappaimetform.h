#ifndef PIKANAPPAIMETFORM_H
#define PIKANAPPAIMETFORM_H

#include <QWidget>

namespace Ui {
    class PikanappaimetForm;
}

class PikanappaimetForm : public QWidget
{
    Q_OBJECT

public:
    explicit PikanappaimetForm(QWidget *parent = 0);
    ~PikanappaimetForm();

private:
    Ui::PikanappaimetForm *ui;
};

#endif // PIKANAPPAIMETFORM_H
