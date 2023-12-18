#include "mainwindow.h"
#include "qapplication.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>

bool setupDb()
{
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS quotes (symbol TEXT PRIMARY KEY COLLATE NOCASE, response TEXT, open REAL, high REAL, low REAL, price REAL, volume INTEGER, latest_trading_date TEXT, previous_close REAL, change REAL, change_percent TEXT, updated_at DATETIME)")) {
        return false;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS watch_lists (name TEXT PRIMARY KEY COLLATE NOCASE, note TEXT, created_at DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        return false;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS watch_list_symbols (watch_list_name TEXT, symbol TEXT, FOREIGN KEY (watch_list_name) REFERENCES watch_lists(name) ON DELETE CASCADE, FOREIGN KEY (symbol) REFERENCES quotes(symbol) ON DELETE CASCADE)")) {
        return false;
    }
    return true;
}

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
    if (!setupDb()) {
        return EXIT_FAILURE;
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
