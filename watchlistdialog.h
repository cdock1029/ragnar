#pragma once

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class WatchListDialog;
}

class WatchListDialog : public QDialog // NOLINT(*-special-member-functions)
{
    Q_OBJECT

    Ui::WatchListDialog* ui;
    QSqlTableModel* m_watchlist_model;

public:
    explicit WatchListDialog(QSqlTableModel* watchlist_model, QWidget* parent = nullptr);
    ~WatchListDialog() override;
};
