#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

#pragma once

namespace arche::data {

struct Ust {
    int id;
    QString cusip;
    QDate issueDate;
    QDate maturityDate;
    double interestRate;
    double highInvestmentRate;
    QString securityTermDayMonth;
    QString securityTermWeekYear;
};

[[nodiscard]] QSqlError saveUST(const QJsonObject& obj);

[[nodiscard]] QSqlError deleteUST(int id);

QList<Ust> listUst();

}
