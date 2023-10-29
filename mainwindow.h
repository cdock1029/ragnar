#pragma once

#include <QMainWindow>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow { // NOLINT(*-special-member-functions)
    Q_OBJECT

    Ui::MainWindow* ui;

    QSqlTableModel* m_watchlist_model;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
};
