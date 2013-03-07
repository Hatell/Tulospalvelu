#ifndef SELAUSFORM_H
#define SELAUSFORM_H

#include <QtGui>

#include "form/utilform.h"

#include "makrot.h"

namespace Ui {
    class SelausForm;
}

class SelausForm : public UtilForm
{
    Q_OBJECT

public:
    explicit SelausForm(QWidget *parent = 0);
    ~SelausForm();

public slots:
    void updateForm();

signals:
    void requestClose(QWidget *);

private slots:
    void on_closeButton_clicked();

private:
    Ui::SelausForm *ui;
};

#endif // SELAUSFORM_H
