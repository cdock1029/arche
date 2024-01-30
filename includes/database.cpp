#include "database.h"

using namespace Qt::StringLiterals;

QSqlError arche::data::saveUST(const QJsonObject& obj)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO ust (cusip, issueDate, maturityDate, interestRate, "
        "highInvestmentRate, securityTermDayMonth, securityTermWeekYear) "
        "VALUES (:cusip, :issueDate, :maturityDate, :interestRate, :highInvestmentRate, "
        u":securityTermDayMonth, :securityTermWeekYear)"_s);
    query.bindValue(u":cusip"_s, obj[u"cusip"_s].toString());
    query.bindValue(u":issueDate"_s,
        obj[u"issueDate"_s].toString());
    query.bindValue(u":maturityDate"_s,
        obj[u"maturityDate"_s].toString());
    query.bindValue(u":interestRate"_s,
        obj[u"interestRate"_s].toString().toDouble());
    query.bindValue(u":highInvestmentRate"_s,
        obj[u"highInvestmentRate"_s].toString().toDouble());
    query.bindValue(u":securityTermDayMonth"_s,
        obj[u"securityTermDayMonth"_s].toString());
    query.bindValue(u":securityTermWeekYear"_s,
        obj[u"securityTermWeekYear"_s].toString());

    if (!query.exec()) {
        return query.lastError();
    }
    return {};
}

QList<arche::data::Ust> arche::data::listUst()
{
    QList<Ust> list;
    QSqlQuery query(u"SELECT * FROM ust"_s);
    while (query.next()) {
        Ust ust;
        ust.id = query.value(u"id"_s).toInt();
        ust.cusip = query.value(u"cusip"_s).toString();
        ust.issueDate = QDate::fromString(query.value(u"issueDate"_s).toString(), Qt::ISODate);
        ust.maturityDate = QDate::fromString(query.value(u"maturityDate"_s).toString(), Qt::ISODate);
        ust.interestRate = query.value(u"interestRate"_s).toDouble();
        ust.highInvestmentRate = query.value(u"highInvestmentRate"_s).toDouble();
        ust.securityTermDayMonth = query.value(u"securityTermDayMonth"_s).toString();
        ust.securityTermWeekYear = query.value(u"securityTermWeekYear"_s).toString();
        list.append(ust);
    }
    return list;
}

QSqlError arche::data::deleteUST(int id)
{
    QSqlQuery query(u"DELETE from ust where id = %1"_s.arg(id));
    if (!query.exec()) {
        return query.lastError();
    }
    return {};
}
