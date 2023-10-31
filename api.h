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

class Api : public QObject { // NOLINT(*-special-member-functions)
    Q_OBJECT

    QNetworkAccessManager* m_network_access_manager;

public:
    explicit Api(QObject* parent = nullptr);
    ~Api() override;

    void getSymbol(const QString& symbol, const std::function<void(const QString& response)>& callback, api::CacheParam = api::CacheParam::USE_CACHE);

signals:

private:
    static QString lookup(const QString& symbol);
    static void save(const QString& symbol, const QString& response);
};
