#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <functional>

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

    QNetworkAccessManager* m_network_access_manager;

public:
    explicit Api(QObject* parent = nullptr);
    ~Api() override;

    void getSymbol(const QString& symbol, const std::function<void(Quote&& quote)>&& callback, api::CacheParam = api::CacheParam::USE_CACHE);

signals:

private:
    static Quote lookup(const QString& symbol);
    static Quote parseQuoteResponse(const QString& response);
    static void save(const Quote& quote);
};
