#include "routes.h"
#include <QCoreApplication>
#include <QHttpServer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <webxx.h>

using namespace Qt::StringLiterals;

constexpr auto CREATE_TABLE = "CREATE TABLE IF NOT EXISTS ust ("
                              "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                              "cusip TEXT NOT NULL COLLATE NOCASE,"
                              "issueDate TEXT NOT NULL,"
                              "maturityDate TEXT NOT NULL,"
                              "interestRate REAL NOT NULL,"
                              "highInvestmentRate REAL NOT NULL,"
                              "securityTermDayMonth TEXT NOT NULL,"
                              "securityTermWeekYear TEXT NOT NULL)"_L1;

constexpr auto CREATE_UNIQUE_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS ust_cusip_issueDate ON ust (cusip, issueDate)"_L1;

int main(int argc, char* argv[])
{
    const QCoreApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s);
    db.setDatabaseName(u"arche.db"_s);
    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError();
        return EXIT_FAILURE;
    }
    QSqlQuery query;
    query.exec(CREATE_TABLE);
    query.exec(CREATE_UNIQUE_INDEX);

    QHttpServer server;

    const Routes routes(server);

    constexpr int PORT = 3000;
    qInfo() << "Listening on :" << PORT;
    server.listen(QHostAddress::Any, PORT);

    return QCoreApplication::exec();
}
