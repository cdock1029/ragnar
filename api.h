#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <functional>

using namespace Qt::Literals::StringLiterals;

struct Quote {
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
    friend QDebug operator<<(QDebug debug, const Quote& c)
    {
        QDebugStateSaver saver(debug);
        debug.nospace() << "Quote(" << c.symbol << ", " << c.response << ", " << c.open << ", " << c.high << ", " << c.low << ", " << c.price << ", " << c.volume << ", " << c.latest_trading_day << ", " << c.previous_close << ", " << c.change << ", " << c.change_percent << ", " << c.updated_at << ")";
        return debug;
    }
};

namespace Api {

enum class CacheParam {
    USE_NETWORK,
    USE_CACHE
};

extern QNetworkAccessManager& getManager();

extern void getSymbol(const QString& symbol, const std::function<void(Quote&& quote)>&& callback, CacheParam = CacheParam::USE_CACHE);

};
