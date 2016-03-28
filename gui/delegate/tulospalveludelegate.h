#ifndef TULOSPALVELUDELEGATE_H
#define TULOSPALVELUDELEGATE_H

#include <QtCore>
#include <QWidget>
#include <QStyledItemDelegate>
#include <QDateTimeEdit>

class TulospalveluDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TulospalveluDelegate(QObject *parent = 0);


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;
    QString displayText(const QVariant &value, const QLocale &locale) const;
/*
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
        */
};

#endif // TULOSPALVELUDELEGATE_H
