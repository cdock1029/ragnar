#include "watchlistdialog.h"
#include "qnamespace.h"
#include "ui_watchlistdialog.h"
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>

using namespace Qt::Literals::StringLiterals;

WatchListDialog::WatchListDialog(QSqlTableModel* watchlist_model, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::WatchListDialog)
    , m_watchlist_model(watchlist_model)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        auto record = m_watchlist_model->record();

        record.setValue(u"name"_s, ui->name_edit->text());
        record.setValue(u"note"_s, ui->note_edit->toPlainText());
        record.remove(2);

        if (m_watchlist_model->insertRecord(-1, record)) {
            m_watchlist_model->submitAll();
            m_watchlist_model->select();
        } else {
            qDebug() << "Error saving record: " << m_watchlist_model->lastError();
        }

        accept();
    });
}

WatchListDialog::~WatchListDialog()
{
    delete ui;
}
