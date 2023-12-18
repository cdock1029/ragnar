#include "api.h"
#include "env.h"
#include "qassert.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

Quote lookup(const QString& symbol)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM quotes WHERE symbol = ? limit 1");
    query.addBindValue(symbol);
    query.exec();

    if (query.first()) {
        Q_ASSERT(symbol == query.value("symbol").toString());
        Quote quote {
            .symbol = symbol,
            .response = query.value("response").toString(),
            .open = query.value("open").toDouble(),
            .high = query.value("high").toDouble(),
            .low = query.value("low").toDouble(),
            .price = query.value("price").toDouble(),
            .volume = query.value("volume").toInt(),
            .latest_trading_day = query.value("latest_trading_date").toString(),
            .previous_close = query.value("previous_close").toDouble(),
            .change = query.value("change").toDouble(),
            .change_percent = query.value("change_percent").toString(),
            .updated_at = query.value("updated_at").toString()

        };
        return quote;
    }
    return Quote {};
}

Quote parseQuoteResponse(const QString& response)
{
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(response.toUtf8(), &error);
    if (json.isNull()) {
        qDebug() << "Json error:" << &error;
        return Quote {};
    }
    QJsonObject global = json["Global Quote"].toObject();
    auto symbol = global["01. symbol"].toString();
    auto open = global["02. open"].toString();
    auto high = global["03. high"].toString();
    auto low = global["04. low"].toString();
    auto price = global["05. price"].toString();
    auto volume = global["06. volume"].toString();
    auto latest_trading_day = global["07. latest trading day"].toString();
    auto previous_close = global["08. previous close"].toString();
    auto change = global["09. change"].toString();
    auto change_percent = global["10. change percent"].toString();

    return Quote { symbol, response, open.toDouble(), high.toDouble(), low.toDouble(), price.toDouble(),
        volume.toInt(), latest_trading_day, previous_close.toDouble(), change.toDouble(), change_percent, "" /*updated_at*/ };
}

void save(const Quote& quote)
{
    qDebug() << "-- save --";
    QSqlQuery query;
    query.prepare("INSERT INTO quotes (symbol, response, open, high, low, price, volume, latest_trading_date, previous_close, change, change_percent) "
                  "values (:symbol, :response, :open, :high, :low, :price, :volume, :latest_trading_date, :previous_close, :change, :change_percent) "
                  "ON CONFLICT(symbol) DO UPDATE "
                  "SET response=excluded.response, open=excluded.open, high=excluded.high, low=excluded.low, price=excluded.price, "
                  "volume=excluded.volume, latest_trading_date=excluded.latest_trading_date, previous_close=excluded.previous_close, change=excluded.change, change_percent=excluded.change_percent,"
                  "updated_at=CURRENT_TIMESTAMP");
    query.bindValue(":symbol", quote.symbol);
    query.bindValue(":response", quote.response);
    query.bindValue(":open", quote.open);
    query.bindValue(":high", quote.high);
    query.bindValue(":low", quote.low);
    query.bindValue(":price", quote.price);
    query.bindValue(":volume", quote.volume);
    query.bindValue(":latest_trading_date", quote.latest_trading_day);
    query.bindValue(":previous_close", quote.previous_close);
    query.bindValue(":change", quote.change);
    query.bindValue(":change_percent", quote.change_percent);
    if (!query.exec()) {
        qWarning() << "save symbol error:" << query.lastError();
        qWarning() << "quote:" << &quote;
    }
}

void Api::getSymbol(const QString& symbol, const std::function<void(Quote&&)>&& callback, CacheParam cache_param)
{
    // define VANTAGE api key in env.h
    static const auto GLOBAL_QUOTE_URL = u"https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=%1&apikey="_s VANTAGE;

    if (cache_param == CacheParam::USE_CACHE) {
        Quote find = lookup(symbol);
        if (!find.symbol.isEmpty()) {
            callback(std::move(find));
            return;
        }
    }
    qDebug() << "getSymbol initiating request for symbol:" << symbol;

    auto request = QNetworkRequest { QUrl { GLOBAL_QUOTE_URL.arg(symbol) } };
    request.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
    // request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    // request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysCache);

    auto* reply = getManager().get(request);

    QObject::connect(reply, &QNetworkReply::errorOccurred, [=]([[maybe_unused]] QNetworkReply::NetworkError error) {
        qWarning() << "NetworkError: " << reply->errorString();
    });

    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            auto fromCache = reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
            if (fromCache.toBool()) {
                qInfo() << "\nREPLY IS from network CACHE!" << '\n';
            }
            const QString response = reply->readAll();
            qInfo() << "QNetworkReply::finished response:" << response;
            Quote quote = parseQuoteResponse(response);
            save(quote);
            callback(std::move(quote));
        } else {
            qInfo() << "finished but with error, not processing response.";
        }
    });
}

QNetworkAccessManager& Api::getManager()
{
    static bool isSetup { false };
    static QNetworkDiskCache diskCache;
    static QNetworkAccessManager manager;
    if (!isSetup) {
        // /home/user/.cache/C14/Ragnar/cache
        diskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + u"/cache/"_s);
        manager.setCache(&diskCache);
        isSetup = true;
    }
    return manager;
}
