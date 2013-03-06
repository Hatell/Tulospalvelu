#include <QtGui/QApplication>
#include "mainwindow.h"

#include "type/tapahtuma.h"

#include "makrot.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QApplication::setOrganizationName(_("PispalanInsinööritoimistoOy"));
    QApplication::setApplicationName(_("Tulospalvelu"));

    // Tarkistetaan valittiinko tapahtuma vai painettiinko esc tai muuta.
    if (Tapahtuma::tapahtuma()->id() == 0) {
        return 0;
    }

    return a.exec();
}
