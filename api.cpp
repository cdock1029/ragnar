#include "api.h"
#include "env.h"
#include <QNetworkReply>
#include <QSqlError>
#include <QSqlQuery>

Api::Api(QObject* parent)
    : QObject { parent }
    , m_network_access_manager { new QNetworkAccessManager }
{
}

Api::~Api()
{
    delete m_network_access_manager;
}

void Api::getSymbol(const QString& symbol, const std::function<void(const QString&)>& callback, api::CacheParam cache_param)
{
    static const auto GLOBAL_QUOTE_URL = u"https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=%1&apikey="_s VANTAGE;

    if (cache_param == api::CacheParam::USE_CACHE) {
        const QString find = lookup(symbol);
        if (!find.isNull() && !find.isEmpty()) {
            callback(find);
            return;
        }
    }
    qDebug() << "getSymbol initiating request for symbol:" << symbol;

    auto request = QNetworkRequest { QUrl { GLOBAL_QUOTE_URL.arg(symbol) } };

    auto* reply = m_network_access_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        const QString data = reply->readAll();
        qInfo() << "QNetworkReply::finished data:" << data;
        save(symbol, data);

        callback(data);
    });

    connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError error) {
        reply->deleteLater();
        qWarning() << "NetworkError: " << error;
    });
}

QString Api::lookup(const QString& symbol)
{
    QSqlQuery query;
    query.prepare("SELECT response FROM quotes WHERE symbol = ? limit 1");
    query.addBindValue(symbol);
    query.exec();

    QString result;
    if (query.first()) {
        result = query.value(0).toString();
    }
    return result;
}

void Api::save(const QString& symbol, const QString& response)
{
    qDebug() << "-- save --";
    QSqlQuery query;
    query.prepare("INSERT INTO quotes (symbol, response) values (?, ?) ON CONFLICT(symbol) DO UPDATE SET response=excluded.response, updated_at=CURRENT_TIMESTAMP");
    query.addBindValue(symbol);
    query.addBindValue(response);
    if (!query.exec()) {
        qWarning() << "save symbol error:" << query.lastError();
        qWarning() << "symbol:" << symbol << ", response:" << response;
    }
}
