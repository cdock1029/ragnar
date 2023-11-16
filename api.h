#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <functional>
#include <memory>

using namespace Qt::Literals::StringLiterals;

namespace api {
enum class CacheParam {
    USE_NETWORK,
    USE_CACHE
};
}

struct Quote { // NOLINT(*-special-member-functions)
    QString symbol;
    QString response;
    double open;
    double high;
    double low;
    double price;
    int volume;
    QString latest_trading_day;
    double previous_close;
    double change;
    QString change_percent;
    QString updated_at;
};

class Api : public QObject { // NOLINT(*-special-member-functions)
    Q_OBJECT

    inline static const std::unique_ptr<QNetworkAccessManager> m_network_access_manager = std::make_unique<QNetworkAccessManager>();

public:
    explicit Api(QObject* parent = nullptr);
    ~Api() override = default;

    static void getSymbol(const QString& symbol, const std::function<void(Quote&& quote)>&& callback, api::CacheParam = api::CacheParam::USE_CACHE);

private:
    static Quote lookup(const QString& symbol);
    static Quote parseQuoteResponse(const QString& response);
    static void save(const Quote& quote);
};
