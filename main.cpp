#include "mainwindow.h"
#include "qapplication.h"

#include <QApplication>
#include <QSqlDatabase>
#include <cstdlib>

int main(int argc, char* argv[])
{
    const QApplication a(argc, argv);
    QApplication::setOrganizationName("C14");
    QApplication::setApplicationName("Ragnar");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ragnar.db");

    if (!db.open()) {
        return EXIT_FAILURE;
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
