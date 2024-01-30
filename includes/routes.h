#include <QHttpServer>
#include <QNetworkAccessManager>
#include <QObject>

#pragma once

class Routes : public QObject {
    Q_OBJECT

    QNetworkAccessManager* m_manager = new QNetworkAccessManager { this };

public:
    explicit Routes(QHttpServer& server, QObject* parent = nullptr);
    ~Routes() override = default;
};
