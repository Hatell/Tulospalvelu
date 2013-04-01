#include "tulosnayttoform.h"
#include "ui_tulosnayttoform.h"

TulosnayttoForm::TulosnayttoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TulosnayttoForm),
    m_settings()
{
    ui->setupUi(this);

    setupFont();

    sqlTulos();
}

TulosnayttoForm::~TulosnayttoForm()
{
    delete ui;
}

void TulosnayttoForm::setupFont()
{
    QFont f = font();

    f.setPointSize(m_settings.value("TulosnayttoForm/pointSize", 30).toInt());
    setFont(f);
}

void TulosnayttoForm::updateForm()
{
    sqlTulos();
}

void TulosnayttoForm::sqlTulos()
{
    QSqlDatabase::database().transaction();

    QSqlQuery query;

    query.prepare(_(
                "SELECT\n"
                "  t.aika,\n"
                "  t.maaliaika,\n"
                "  k.nimi AS kilpailija,\n"
                "  s.nimi AS sarja\n"
                "FROM tulos AS t\n"
                "  JOIN kilpailija AS k ON k.id = t.kilpailija\n"
                "  JOIN sarja AS s ON s.id = t.sarja\n"
                "WHERE t.tapahtuma = ?\n"
                "ORDER BY t.id DESC\n"
                "LIMIT %1\n"
    ).arg(m_settings.value("TulosnayttoForm/tulosLimit", 3).toInt()));

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query,);

    QStringList str;

    while (query.next()) {
        QSqlRecord r = query.record();

        str <<
               r.value("kilpailija").toString() +
               "\nSarja:\t" +
               r.value("sarja").toString() +
               "\nTulos:\t" +
               r.value("aika").toTime().toString("HH:mm:ss") +
               "\t      Maaliaika:\t" + r.value("maaliaika").toDateTime().time().toString("HH:mm:ss");
    }

    ui->label->setText(str.join("\n\n"));

    QSqlDatabase::database().commit();
}

void TulosnayttoForm::show()
{
    setupFont();
    sqlTulos();

    QWidget::show();
}
