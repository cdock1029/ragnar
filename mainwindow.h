#pragma once

#include "api.h"
#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow { // NOLINT(*-special-member-functions)
    Q_OBJECT

    Ui::MainWindow* ui;

    QSqlTableModel* m_watchlists_model;
    QSqlTableModel* m_watch_list_symbols;
    Api* m_api;

    static constexpr int ACTIVE_WATCHLIST_LABEL_FONT_SIZE { 18 };

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void writeSettings();
    void readSettings();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event) override;
};
