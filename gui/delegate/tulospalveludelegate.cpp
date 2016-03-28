#include "tulospalveludelegate.h"

TulospalveluDelegate::TulospalveluDelegate(QObject *parent):
  QStyledItemDelegate(parent)
{

}

QWidget *TulospalveluDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);

    QDateTimeEdit *datetimeedit = qobject_cast<QDateTimeEdit*>(editor);
    if (datetimeedit) {
        datetimeedit->setDisplayFormat("dd.MM.yyyy HH:mm:ss");
        datetimeedit->setCalendarPopup(true);
    }

    return editor;
}

QString TulospalveluDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if (value.toDateTime().isValid()) {
        return value.toDateTime().toString("dd.MM.yyyy HH:mm:ss");
    }

    return QStyledItemDelegate::displayText(value, locale);
}
