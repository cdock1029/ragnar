#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "api.h"
#include "watchlistdialog.h"

#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QSplitter>
#include <QSqlQuery>
#include <QSqlRecord>

using namespace Qt::Literals::StringLiterals;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_watchlists_model { new QSqlTableModel { this } }
    , m_watch_list_query { new QSqlQueryModel { this } }
{
    ui->setupUi(this);
    setWindowTitle(u"Ragnar"_s);

    auto* splitter = new QSplitter { this };
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(ui->watchlists_frame);
    splitter->addWidget(ui->active_watchlist_frame);

    // todo: fix width of splitter
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* layout = new QHBoxLayout { ui->centralwidget };
    layout->addWidget(splitter, 0, Qt::AlignLeft);
    ui->centralwidget->setLayout(layout);

    m_watchlists_model->setTable(u"watch_lists"_s);
    m_watchlists_model->select();

    auto* watch_list_view = ui->watch_lists_view;
    watch_list_view->setModel(m_watchlists_model);
    watch_list_view->setModelColumn(m_watchlists_model->fieldIndex(u"name"_s));
    watch_list_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    watch_list_view->setContextMenuPolicy(Qt::CustomContextMenu);

    //
    auto* active_watchlist_table = ui->active_watchlist_table;
    active_watchlist_table->setModel(m_watch_list_query);
    active_watchlist_table->verticalHeader()->hide();

    active_watchlist_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    active_watchlist_table->horizontalHeader()->setStretchLastSection(true);
    //

    auto font = ui->active_watchlist_label->font();
    font.setPointSize(ACTIVE_WATCHLIST_LABEL_FONT_SIZE);
    ui->active_watchlist_label->setFont(font);

    connect(ui->action_Quit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionDelete_Watchlist, &QAction::triggered, this, [=, this]() {
        auto* selection = watch_list_view->selectionModel();
        auto idx = selection->currentIndex();
        m_watchlists_model->removeRow(idx.row());
        m_watchlists_model->submitAll();
        m_watchlists_model->select();
    });

    connect(ui->actionNew_Watchlist, &QAction::triggered, this, [this]([[maybe_unused]] bool checked) {
        auto* watchListDialog = new WatchListDialog(m_watchlists_model, this);
        watchListDialog->setWindowTitle(u"New Watchlist"_s);
        watchListDialog->show();
    });

    connect(ui->toolButton, &QToolButton::clicked, ui->actionNew_Watchlist, &QAction::triggered);

    connect(watch_list_view, &QListView::customContextMenuRequested, this, [=, this](QPoint pos) {
        auto* menu = new QMenu { this };
        menu->addAction(ui->actionDelete_Watchlist);
        menu->popup(watch_list_view->viewport()->mapToGlobal(pos));
    });

    connect(watch_list_view->selectionModel(), &QItemSelectionModel::currentChanged, this, [=, this](const QModelIndex& current, [[maybe_unused]] const QModelIndex& previous) {
        const auto name = current.data().toString();
        ui->active_watchlist_label->setText(name);

        m_watch_list_query->setQuery(u"SELECT q.symbol, q.price from quotes q "
                                     "inner join watch_list_symbols ws on q.symbol = ws.symbol "
                                     "inner join watch_lists w on w.name = ws.watch_list_name "
                                     "where ws.watch_list_name = '%1'"_s.arg(name));
    });

    watch_list_view->setCurrentIndex(watch_list_view->model()->index(0, m_watchlists_model->fieldIndex(u"name"_s)));

    connect(ui->api_btn, &QPushButton::clicked, this, []() {
        Api::getSymbol(
            u"ALLY"_s, [](const Quote& quote) {
                qDebug() << "Quote inside main window connect:" << quote;
            },
            Api::CacheParam::USE_CACHE);
    });
    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeSettings()
{
    auto* layout = ui->centralwidget->layout();
    auto* splitter = qobject_cast<QSplitter*>(layout->itemAt(0)->widget());

    QSettings settings;
    settings.beginGroup(u"MainWindow"_s);
    settings.setValue(u"geometry"_s, saveGeometry());
    settings.setValue(u"splitter.state"_s, splitter->saveState());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup(u"MainWindow"_s);
    const auto geometry = settings.value(u"geometry"_s, QByteArray()).toByteArray();
    const auto splitter_state = settings.value(u"splitter.state"_s, QByteArray()).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    if (!splitter_state.isEmpty()) {
        auto* layout = ui->centralwidget->layout();
        auto* splitter = qobject_cast<QSplitter*>(layout->itemAt(0)->widget());
        splitter->restoreState(splitter_state);
    }
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    event->accept();
}
