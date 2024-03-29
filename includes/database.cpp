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

std::vector<arche::data::Ust> arche::data::listUst()
{
    QSqlQuery query(u"SELECT * FROM ust"_s);
    std::vector<Ust> list;
    if (!query.exec()) {
        qDebug() << "listUst query error: " << query.lastError();
        return list;
    }
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
        list.push_back(ust);
    }
    return list;
}

QSqlError arche::data::deleteUST(int id)
{
    QSqlQuery query;
    query.prepare(u"DELETE from ust where id = ?"_s);
    query.bindValue(0, id);
    if (!query.exec()) {
        return query.lastError();
    }
    if (query.numRowsAffected() < 1) {
        return QSqlError { QString {}, u"id %1 not found"_s.arg(id) };
    }
    return {};
}
