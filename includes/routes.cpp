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
const auto TEXT_HTML = "text/html"_ba;
const auto TEXT_PLAIN = "text/plain"_ba;

Routes::Routes(QHttpServer& server, QObject* parent)
    : QObject(parent)
{
    using namespace htmx;
    using Webxx::render;
    using Method = QHttpServerRequest::Method;

    server.route(u"/"_s, Method::Get, [](const QHttpServerRequest& req) -> QHttpServerResponse {
        auto comp = isHxRequest(req)
            ? render(components::Home {})
            : render(components::Layout { components::Home {} });
        return { TEXT_HTML, comp.data() };
    });

    server.route(u"/"_s, Method::Post, [this](const QHttpServerRequest& req, QHttpServerResponder&& responder) {
        QUrlQuery q { QString::fromLatin1(req.body()) };

        auto cusip = q.queryItemValue(u"cusip"_s);
        auto issueDate = QDate::fromString(q.queryItemValue(u"issueDate"_s, QUrl::FullyDecoded), Qt::ISODate);

        auto url = TD_SECURITY_URL.arg(cusip, issueDate.toString(u"MM/dd/yyyy"_s));
        auto* reply = m_manager->get(QNetworkRequest { url });

        connect(reply, &QNetworkReply::finished, [reply, responder = std::move(responder)]() {
            reply->deleteLater();
            const auto obj = QJsonDocument::fromJson(reply->readAll()).object();
            if (obj.isEmpty()) {
                const_cast<QHttpServerResponder&>(responder).sendResponse({ QHttpServerResponse::StatusCode::NotFound });
                return;
            }
            auto err = arche::data::saveUST(obj);
            if (err.isValid()) {
                qDebug() << "saveUST err:" << err;
                if ((err.nativeErrorCode() == PRIMARY_KEY_ERROR || err.nativeErrorCode() == UNIQUE_CONSTRAINT_ERROR)) {
                    qDebug() << "caught unique error";
                    const_cast<QHttpServerResponder&>(responder).sendResponse({ TEXT_PLAIN,
                        "UST already saved"_ba, QHttpServerResponse::StatusCode::Conflict });
                    return;
                }
            }
            const_cast<QHttpServerResponder&>(responder).sendResponse(hxRedirect({ TEXT_PLAIN, QByteArray {}, QHttpServerResponse::StatusCode::Created }, "/treasuries"));
        });
    });

    server.route(u"/test"_s, Method::Post, []() {
        return QHttpServerResponse { TEXT_PLAIN, "Post result" };
    });
    server.route(u"/test"_s, Method::Delete, []() {
        // return QHttpServerResponse { TEXT_PLAIN, "Delete result" };
        return QHttpServerResponse { QHttpServerResponse::StatusCode::Ok };
    });

    server.route(u"/treasuries"_s, Method::Get, [](const QHttpServerRequest& req) -> QHttpServerResponse {
        auto treasuries = arche::data::listUst();
        auto comp = isHxRequest(req)
            ? render(components::Treasuries { treasuries })
            : render(components::Layout { components::Treasuries { treasuries } });
        return { TEXT_HTML, comp.data() };
    });

    server.route(u"/treasuries/<arg>"_s, Method::Delete, [](int id, QHttpServerResponder&& responder) {
        auto err = arche::data::deleteUST(id);
        if (err.isValid()) {
            qDebug() << "deleteUST for id " << id << " err:" << err.text();
            const_cast<QHttpServerResponder&>(responder).sendResponse({ QHttpServerResponse::StatusCode::InternalServerError });
            return;
        }
        const_cast<QHttpServerResponder&>(responder).sendResponse({ QHttpServerResponse::StatusCode::Ok });
    });

    server.route(u"/search"_s, Method::Post, [this](const QHttpServerRequest& req, QHttpServerResponder&& responder) {
        QUrlQuery q { QString::fromLatin1(req.body()) };
        auto cusip = q.queryItemValue(u"cusip"_s).trimmed();
        if (cusip.isEmpty()) {
            responder.sendResponse({ QHttpServerResponse::StatusCode::BadRequest });
            return;
        }
        auto url = TD_SEARCH_URL.arg(cusip);
        auto* reply = m_manager->get(QNetworkRequest { url });

        connect(reply, &QNetworkReply::finished, [reply, responder = std::move(responder)]() {
            reply->deleteLater();
            const auto arr = QJsonDocument::fromJson(reply->readAll()).array();
            const_cast<QHttpServerResponder&>(responder).sendResponse({ TEXT_HTML,
                render(components::Layout { components::SearchResults { arr } }).data() });
        });
    });
}
