#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qaction.h"
#include "qmenu.h"
#include "qnamespace.h"
#include "watchlistdialog.h"

using namespace Qt::Literals::StringLiterals;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_watchlist_model { new QSqlTableModel { this } }
{
    ui->setupUi(this);

    setWindowTitle(u"Ragnar"_s);

    m_watchlist_model->setTable("watch_lists");
    m_watchlist_model->select();
    auto* watch_list_view = ui->watch_lists_view;
    watch_list_view->setModel(m_watchlist_model);
    watch_list_view->setModelColumn(0);
    watch_list_view->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->action_Quit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionDelete_Watchlist, &QAction::triggered, this, [=, this]() {
        auto* selection = watch_list_view->selectionModel();
        auto idx = selection->currentIndex();
        m_watchlist_model->removeRow(idx.row());
        m_watchlist_model->submitAll();
        m_watchlist_model->select();
    });

    connect(ui->actionNew_Watchlist, &QAction::triggered, this, [this]([[maybe_unused]] bool checked) {
        auto* watchListDialog = new WatchListDialog(m_watchlist_model, this);
        watchListDialog->setWindowTitle(u"New Watchlist"_s);
        watchListDialog->show();
    });

    connect(ui->toolButton, &QToolButton::clicked, ui->actionNew_Watchlist, &QAction::triggered);

    connect(watch_list_view, &QListView::customContextMenuRequested, this, [=, this](QPoint pos) {
        auto* menu = new QMenu { this };
        menu->addAction(ui->actionDelete_Watchlist);
        menu->popup(watch_list_view->viewport()->mapToGlobal(pos));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
