#include "dbhelper.h"
#include <QDateTime>
#include <QRegularExpression>

bool DBHelper::login_sql(const QString &username, const QString &password){
    if (!db.isOpen()) {
        QMessageBox::critical(nullptr, "错误", "数据库未连接！");
        return false;
    }

    QSqlQuery query(db);
    QString sql = "SELECT * FROM Users WHERE user_name = :user_name AND user_password = :user_password";
    query.prepare(sql);
    query.bindValue(":user_name", username);
    query.bindValue(":user_password", password);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "查询失败", query.lastError().text());
        return false;
    }
    return query.next();
}

int DBHelper::get_userid_sql(const QString &username){
    if (!db.isOpen()) {
        QMessageBox::critical(nullptr, "错误", "数据库未连接！");
        return 0;
    }

    QSqlQuery query(db);
    QString sql = "SELECT id FROM Users WHERE user_name = :user_name";
    query.prepare(sql);
    query.bindValue(":user_name",username);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    else return 0;
}

// 带个人信息的注册函数
bool DBHelper::register_sql(const QString &username, const QString &password,
                            const QString &realName, const QString &idCard) {
    if (!db.isOpen()) {
        QMessageBox::critical(nullptr, "错误", "数据库未连接！");
        return false;
    }

    if (username.isEmpty() || password.isEmpty() || realName.isEmpty() || idCard.isEmpty()) {
        QMessageBox::warning(nullptr, "注册失败", "所有字段不能为空！");
        return false;
    }

    QRegularExpression idRegex("^[1-9]\\d{5}(18|19|20)\\d{2}((0[1-9])|(1[0-2]))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$");
    if (!idRegex.match(idCard).hasMatch()) {
        QMessageBox::warning(nullptr, "注册失败", "身份证号格式不正确！");
        return false;
    }

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM Users WHERE user_name = :user_name OR real_id = :real_id");
    checkQuery.bindValue(":user_name", username);
    checkQuery.bindValue(":real_id", idCard);
    if (!checkQuery.exec()) {
        QMessageBox::critical(nullptr, "查询失败", checkQuery.lastError().text());
        return false;
    }
    if (checkQuery.next()) {
        QMessageBox::warning(nullptr, "注册失败", "用户名或身份证号已被占用！");
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare(R"(
        INSERT INTO Users (user_name, user_password, real_name, real_id, role)
        VALUES (:user_name, :user_password, :real_name, :real_id, 0)
    )");
    insertQuery.bindValue(":user_name", username);
    insertQuery.bindValue(":user_password", password);
    insertQuery.bindValue(":real_name", realName);
    insertQuery.bindValue(":real_id", idCard);

    if (!insertQuery.exec()) {
        QMessageBox::critical(nullptr, "注册失败", insertQuery.lastError().text());
        return false;
    }

    return true;
}

//注册
bool DBHelper::register_sql(const QString &username, const QString &password) {
    if (!db.isOpen()) {
        QMessageBox::critical(nullptr, "错误", "数据库未连接！");
        return false;
    }

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM Users WHERE user_name = :user_name");
    checkQuery.bindValue(":user_name", username);
    if (!checkQuery.exec()) {
        QMessageBox::critical(nullptr, "查询失败", checkQuery.lastError().text());
        return false;
    }
    if (checkQuery.next()) {
        QMessageBox::warning(nullptr, "注册失败", "用户名已被占用！");
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare(R"(
        INSERT INTO Users (user_name, user_password, role)
        VALUES (:user_name, :user_password, 0)
    )");
    insertQuery.bindValue(":user_name", username);
    insertQuery.bindValue(":user_password", password);

    if (!insertQuery.exec()) {
        QMessageBox::critical(nullptr, "注册失败", insertQuery.lastError().text());
        return false;
    }

    return true;
}

// 通过用户ID获取个人信息
QMap<QString, QVariant> DBHelper::getUserInfoById(int userId) {
    QMap<QString, QVariant> userInfo;
    if (!db.isOpen() || userId <= 0) {
        QMessageBox::critical(nullptr, "错误", "数据库未连接或用户ID无效！");
        return userInfo;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT user_name, real_name, real_id, role
        FROM Users
        WHERE id = :user_id
    )");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "查询失败", query.lastError().text());
        return userInfo;
    }

    if (query.next()) {
        userInfo["user_name"] = query.value("user_name").toString();
        userInfo["real_name"] = query.value("real_name").toString();
        userInfo["real_id"] = query.value("real_id").toString();
        userInfo["role"] = query.value("role").toInt();
    } else {
        QMessageBox::warning(nullptr, "提示", "未查询到该用户的个人信息！");
    }

    return userInfo;
}

// 修改用户个人信息
bool DBHelper::updateUserInfo(int userId, const QString &newUserName,const QString &newRealName, const QString &newIdCard) {
    if (!db.isOpen() || userId <= 0) {
        QMessageBox::critical(nullptr, "错误", "数据库未连接或用户ID无效！");
        return false;
    }

    if (newRealName.isEmpty() || newIdCard.isEmpty()) {
        QMessageBox::warning(nullptr, "修改失败", "真实姓名和身份证号不能为空！");
        return false;
    }

    QRegularExpression idRegex("^[1-9]\\d{5}(18|19|20)\\d{2}((0[1-9])|(1[0-2]))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$");
    if (!idRegex.match(newIdCard).hasMatch()) {
        QMessageBox::warning(nullptr, "修改失败", "身份证号格式不正确！");
        return false;
    }

    QSqlQuery checkQuery(db);
    checkQuery.prepare(R"(
        SELECT id FROM Users
        WHERE real_id = :real_id AND id != :user_id
    )");
    checkQuery.bindValue(":real_id", newIdCard);
    checkQuery.bindValue(":user_id", userId);
    if (!checkQuery.exec()) {
        QMessageBox::critical(nullptr, "查询失败", checkQuery.lastError().text());
        return false;
    }
    if (checkQuery.next()) {
        QMessageBox::warning(nullptr, "修改失败", "该身份证号已被其他用户使用！");
        return false;
    }

    QSqlQuery updateQuery(db);
    updateQuery.prepare(R"(
        UPDATE Users
        SET real_name = :real_name, real_id = :real_id, user_name = :user_name
        WHERE id = :user_id
    )");
    updateQuery.bindValue(":real_name", newRealName);
    updateQuery.bindValue(":real_id", newIdCard);
    updateQuery.bindValue(":user_name", newUserName);
    updateQuery.bindValue(":user_id", userId);

    if (!updateQuery.exec()) {
        QMessageBox::critical(nullptr, "修改失败", updateQuery.lastError().text());
        return false;
    }

    if (updateQuery.numRowsAffected() > 0) {
        QMessageBox::information(nullptr, "成功", "个人信息修改成功！");
        return true;
    } else {
        QMessageBox::warning(nullptr, "提示", "未修改任何信息（数据无变化）！");
        return false;
    }
}

// 航班查询
QList<QMap<QString, QVariant>> DBHelper::queryFlights_sql()
{
    return queryFlights_sql("", "", QDateTime());
}

QList<QMap<QString, QVariant>> DBHelper::queryFlights_sql(const QString &departCity,
                                                          const QString &arriveCity,
                                                          const QDateTime &departTime)
{
    QList<QMap<QString, QVariant>> flightList;
    if (!db.isOpen()) {
        QMessageBox::critical(nullptr, "错误", "数据库未连接！");
        return flightList;
    }

    QString sql = "SELECT * FROM Flights WHERE 1=1";
    QList<QVariant> params;

    if (!departCity.isEmpty()) {
        sql += " AND depart_city = ?";
        params.append(departCity);
    }
    if (!arriveCity.isEmpty()) {
        sql += " AND arrive_city = ?";
        params.append(arriveCity);
    }
    if (departTime.isValid()) {
        sql += " AND DATE(depart_time) = ?";
        params.append(departTime.date().toString("yyyy-MM-dd"));
    }

    sql += " ORDER BY depart_time ASC";

    QSqlQuery query(db);
    query.prepare(sql);
    for (int i = 0; i < params.size(); i++) {
        query.bindValue(i, params[i]);
    }

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "查询失败", query.lastError().text());
        return flightList;
    }

    while (query.next()) {
        QMap<QString, QVariant> flight;
        flight["flight_id"] = query.value("flight_id").toString();
        flight["depart_city"] = query.value("depart_city").toString();
        flight["arrive_city"] = query.value("arrive_city").toString();
        flight["depart_time"] = query.value("depart_time").toDateTime();
        flight["arrive_time"] = query.value("arrive_time").toDateTime();
        flight["price"] = query.value("price").toInt();
        flight["remaining_seats"] = query.value("remaining_seats").toInt();
        flight["total_seats"] = query.value("total_seats").toInt();
        flightList.append(flight);
    }
    return flightList;
}

int DBHelper::getFlightIdByFlightNo(const QString& flightNo) {
    if (!db.isOpen()) return -1;
    QSqlQuery query(db);
    query.prepare("SELECT id FROM Flights WHERE flight_id = ?");
    query.addBindValue(flightNo);

    int id = -1;
    if (query.exec()) {
        if (query.next()) {
            id = query.value(0).toInt();
        }
    } else {
        qDebug() << "getFlightIdByFlightNo Error:" << query.lastError().text();
    }
    query.finish();
    return id;
}

// 插入订单
bool DBHelper::insertOrder(int userId, int flightId, const QString& seatNum) {
    if (!db.isOpen() || userId <= 0 || flightId <= 0 || seatNum.isEmpty()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("INSERT INTO Orders (user_id, flight_id, order_time, seat_num, status) VALUES (?, ?, ?, ?, 1)");
    query.addBindValue(userId);
    query.addBindValue(flightId);
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue(seatNum);
    return query.exec();
}

// 通过用户ID查订单
QList<QMap<QString, QVariant>> DBHelper::getOrdersByUserId(int userId) {
    QList<QMap<QString, QVariant>> orderList;
    if (!db.isOpen() || userId <= 0) {
        return orderList;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT o.id, o.flight_id, o.order_time, o.seat_num, o.status,
               f.flight_id AS flight_no, f.depart_city, f.arrive_city,
               f.price, f.depart_time
        FROM Orders o
        LEFT JOIN Flights f ON o.flight_id = f.id
        WHERE o.user_id = ? AND o.status=1
        ORDER BY o.order_time DESC
    )");
    query.addBindValue(userId);

    if (query.exec()) {
        while (query.next()) {
            QMap<QString, QVariant> order;
            order["order_id"] = query.value("id").toInt();
            order["flight_id"] = query.value("flight_id").toInt();
            order["flight_no"] = query.value("flight_no").toString();
            order["depart_city"] = query.value("depart_city").toString();
            order["arrive_city"] = query.value("arrive_city").toString();
            order["order_time"] = query.value("order_time").toDateTime();

            order["depart_time"] = query.value("depart_time").toDateTime();

            order["seat_num"] = query.value("seat_num").toString();
            order["price"] = query.value("price").toInt();
            order["status"] = query.value("status").toInt();
            orderList.append(order);
        }
    }
    return orderList;
}

// 新增航班
bool DBHelper::addFlight(const QString& flightNo, const QString& departCity, const QString& arriveCity,
                         const QDateTime& departTime, const QDateTime& arriveTime,
                         int totalSeats, int remainSeats, int price) {
    if (!db.isOpen() || flightNo.isEmpty() || departCity.isEmpty() || arriveCity.isEmpty()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO Flights (flight_id, depart_city, arrive_city, depart_time, arrive_time,
                             total_seats, remaining_seats, price)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");
    query.addBindValue(flightNo);
    query.addBindValue(departCity);
    query.addBindValue(arriveCity);
    query.addBindValue(departTime);
    query.addBindValue(arriveTime);
    query.addBindValue(totalSeats);
    query.addBindValue(remainSeats);
    query.addBindValue(price);
    return query.exec();
}

// 修改航班
bool DBHelper::updateFlight(const QString& flightNo, const QString& departCity, const QString& arriveCity,
                            const QDateTime& departTime, const QDateTime& arriveTime,
                            int totalSeats, int remainSeats, int price) {
    if (!db.isOpen() || flightNo.isEmpty()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE Flights
        SET depart_city = ?, arrive_city = ?, depart_time = ?, arrive_time = ?,
            total_seats = ?, remaining_seats = ?, price = ?
        WHERE flight_id = ?
    )");
    query.addBindValue(departCity);
    query.addBindValue(arriveCity);
    query.addBindValue(departTime);
    query.addBindValue(arriveTime);
    query.addBindValue(totalSeats);
    query.addBindValue(remainSeats);
    query.addBindValue(price);
    query.addBindValue(flightNo);
    return query.exec() && query.numRowsAffected() > 0;
}

// 删除航班
bool DBHelper::deleteFlightByNo(const QString& flightNo) {
    if (!db.isOpen() || flightNo.isEmpty()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("DELETE FROM Flights WHERE flight_id = ?");
    query.addBindValue(flightNo);
    return query.exec() && query.numRowsAffected() > 0;
}

// 退票
bool DBHelper::cancelOrder(int orderId)
{
    if (!db.isOpen() || orderId <= 0)
        return false;

    QSqlQuery query(db);
    query.prepare("SELECT flight_id FROM Orders WHERE id = ? AND status = 1");
    query.addBindValue(orderId);
    if (!query.exec() || !query.next())
        return false;

    int flightId = query.value(0).toInt();

    db.transaction();

    QSqlQuery updateOrder(db);
    updateOrder.prepare("UPDATE Orders SET status = 0 WHERE id = ?");
    updateOrder.addBindValue(orderId);
    if (!updateOrder.exec()) {
        db.rollback();
        return false;
    }

    QSqlQuery updateFlight(db);
    updateFlight.prepare("UPDATE Flights SET remaining_seats = remaining_seats + 1 WHERE id = ?");
    updateFlight.addBindValue(flightId);
    if (!updateFlight.exec()) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}
