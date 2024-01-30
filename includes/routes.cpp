#include "routes.h"
#include "components.h"
#include "database.h"
#include "htmx.h"
#include <QJsonDocument>
#include <QNetworkReply>
#include <QSqlQuery>
#include <webxx.h>

using namespace Qt::StringLiterals;

// /912797HY6/12/12/2023?format=json
// /cusip/mm/dd/yyy?format=json
constexpr auto TD_SECURITY_URL = "https://www.treasurydirect.gov/TA_WS/securities/%1/%2?format=json"_L1;
constexpr auto TD_SEARCH_URL = "https://www.treasurydirect.gov/TA_WS/securities/search?format=json&cusip=%1"_L1;
constexpr auto PRIMARY_KEY_ERROR = "1555"_L1;
constexpr auto UNIQUE_CONSTRAINT_ERROR = "2067"_L1;

Routes::Routes(QHttpServer& server, QObject* parent)
    : QObject(parent)
{
    using namespace htmx;
    using M = QHttpServerRequest::Method;

    server.route(u"/"_s, M::Get, [](const QHttpServerRequest& req) {
        auto comp = isHxRequest(req) ? Webxx::render(components::Home {}) : Webxx::render(components::Layout { components::Home {} });
        auto res = QHttpServerResponse { "text/html"_ba, comp.data(), QHttpServerResponse::StatusCode::Ok };
        return res;
    });

    server.route(u"/"_s, M::Post, [this](const QHttpServerRequest& req, QHttpServerResponder&& responder) {
        QUrlQuery q { QString::fromLocal8Bit(req.body()) };

        auto cusip = q.queryItemValue(u"cusip"_s);
        auto issueDate = QDate::fromString(q.queryItemValue(u"issueDate"_s, QUrl::FullyDecoded), Qt::ISODate);

        auto url = TD_SECURITY_URL.arg(cusip, issueDate.toString(u"MM/dd/yyyy"_s));
        auto* reply = m_manager->get(QNetworkRequest { url });

        connect(reply, &QNetworkReply::finished, [reply, responder = std::move(responder)]() {
            reply->deleteLater();
            const auto obj = QJsonDocument::fromJson(reply->readAll()).object();
            if (obj.isEmpty()) {
                const auto res = QHttpServerResponse { QHttpServerResponse::StatusCode::NotFound };
                const_cast<QHttpServerResponder&>(responder).sendResponse(res);
                return;
            }
            auto err = arche::data::saveUST(obj);
            if (err.isValid()) {
                qDebug() << "saveUST err:" << err;
                if ((err.nativeErrorCode() == PRIMARY_KEY_ERROR || err.nativeErrorCode() == UNIQUE_CONSTRAINT_ERROR)) {
                    qDebug() << "caught unique error";
                    auto res = QHttpServerResponse { "text/plain"_ba, "UST already saved"_ba, QHttpServerResponse::StatusCode::Conflict };
                    const_cast<QHttpServerResponder&>(responder).sendResponse(res);
                    return;
                }
            }
            auto res = QHttpServerResponse { "text/plain"_ba, QByteArray {}, QHttpServerResponse::StatusCode::NoContent };
            const_cast<QHttpServerResponder&>(responder).sendResponse(hxRedirect(res, "/treasuries"));
        });
    });

    server.route(u"/treasuries"_s, M::Get, [](const QHttpServerRequest& req) {
        auto treasuries = arche::data::listUst();
        auto comp = isHxRequest(req) ? Webxx::render(components::Treasuries { treasuries }) : Webxx::render(components::Layout { components::Treasuries { treasuries } });
        auto res = QHttpServerResponse { "text/html"_ba, comp.data(), QHttpServerResponse::StatusCode::Ok };
        return res;
    });

    server.route(u"/treasuries"_s, M::Delete, [](const QHttpServerRequest& req, QHttpServerResponder&& responder) {
        QUrlQuery q { QString::fromLocal8Bit(req.body()) };
        auto id = q.queryItemValue(u"id"_s).toInt();
        auto err = arche::data::deleteUST(id);
        if (err.isValid()) {
            qDebug() << "deleteUST for id " << id << " err:" << err.text();
            const_cast<QHttpServerResponder&>(responder).sendResponse({ QHttpServerResponse::StatusCode::InternalServerError });
            return;
        }
        auto treasuries = arche::data::listUst();
        auto res = QHttpServerResponse { "text/html"_ba, Webxx::render(components::Treasuries { treasuries }).data(), QHttpServerResponse::StatusCode::Ok };
        const_cast<QHttpServerResponder&>(responder).sendResponse(res);
    });

    server.route(u"/search"_s, M::Post, [this](const QHttpServerRequest& req, QHttpServerResponder&& responder) {
        QUrlQuery q { QString::fromLocal8Bit(req.body()) };
        auto cusip = q.queryItemValue(u"cusip"_s).trimmed();
        if (cusip.isEmpty()) {
            responder.sendResponse(QHttpServerResponse { QHttpServerResponse::StatusCode::BadRequest });
            return;
        }
        auto url = TD_SEARCH_URL.arg(cusip);
        auto* reply = m_manager->get(QNetworkRequest { url });

        connect(reply, &QNetworkReply::finished, [reply, responder = std::move(responder)]() {
            reply->deleteLater();
            const auto arr = QJsonDocument::fromJson(reply->readAll()).array();
            const auto res = QHttpServerResponse { "text/html"_ba, Webxx::render(components::Layout { components::SearchResults { arr } }).data(), QHttpServerResponse::StatusCode::Ok };
            const_cast<QHttpServerResponder&>(responder).sendResponse(res);
        });
    });
}
